# $Id: squirrel_export.awk 15481 2009-02-14 20:03:06Z yexo $
#
# Awk script to automatically generate the code needed
# to export the AI API to Squirrel.
#
# Note that arrays are 1 based...
#

# Simple insertion sort.
function array_sort(ARRAY, ELEMENTS, temp, i, j) {
	for (i = 2; i <= ELEMENTS; i++)
		for (j = i; ARRAY[j - 1] > ARRAY[j]; --j) {
			temp = ARRAY[j]
			ARRAY[j] = ARRAY[j - 1]
			ARRAY[j - 1] = temp
	}
	return
}

function dump_class_templates(name) {
	print "	template <> "       name " *GetParam(ForceType<"       name " *>, HSQUIRRELVM vm, int index, SQAutoFreePointers *ptr);"
	print "	template <> "       name " &GetParam(ForceType<"       name " &>, HSQUIRRELVM vm, int index, SQAutoFreePointers *ptr);"
	print "	template <> const " name " *GetParam(ForceType<const " name " *>, HSQUIRRELVM vm, int index, SQAutoFreePointers *ptr);"
	print "	template <> const " name " &GetParam(ForceType<const " name " &>, HSQUIRRELVM vm, int index, SQAutoFreePointers *ptr);"
	if (name == "AIEvent") {
		print "	template <> int Return<" name " *>(HSQUIRRELVM vm, " name " *res);"
	} else {
		print "	template <> int Return<" name " *>(HSQUIRRELVM vm, " name " *res);"
	}
}

BEGIN {
	enum_size = 0
	enum_value_size = 0
	enum_string_to_error_size = 0
	enum_error_to_string_size = 0
	struct_size = 0
	method_size = 0
	static_method_size = 0
	virtual_class = "false"
	super_cls = ""
	cls = ""
	start_squirrel_define_on_next_line = "false"
	cls_level = 0
	RS = "\r|\n"
}

END {
  print ""
	print "#endif  /* " toupper(includestr) "_SQH */"  
}

/@file/ {
	includestr = $3
	sub(/\./, "_", includestr)

	# Break it in two lines, so SVN doesn't replace it
	printf "/* $I"
	print "d$ */"
	print "/* THIS FILE IS AUTO-GENERATED; PLEASE DO NOT ALTER MANUALLY */"
	print ""
	print "#ifndef " toupper(includestr) "_SQH"	
	print "#define " toupper(includestr) "_SQH"	
	print ""	
	print "#include \"" $3 "\""
}

# Remove the old squirrel stuff
/#ifdef DEFINE_SQUIRREL_CLASS/ { squirrel_stuff = "true";  next; }
/^#endif \/\* DEFINE_SQUIRREL_CLASS \*\// { if (squirrel_stuff == "true") { squirrel_stuff = "false"; next; } }
{ if (squirrel_stuff == "true") next; }

# Ignore forward declarations of classes
/^(	*)class(.*);/ { next; }
# We only want to have public functions exported for now
/^(	*)class/     {
	if (cls_level == 0) {
		public = "false"
		cls_param[0] = ""
		cls_param[1] = 1
		cls_param[2] = "x"
		cls = $2
		if (match($4, "public") || match($4, "protected") || match($4, "private")) {
			super_cls = $5
		} else {
			super_cls = $4
		}
	} else if (cls_level == 1) {
		struct_size++
		structs[struct_size] = cls "::" $2
	}
	cls_level++
	next
}
/^(	*)public/    { if (cls_level == 1) public = "true";  next; }
/^(	*)protected/ { if (cls_level == 1) public = "false"; next; }
/^(	*)private/   { if (cls_level == 1) public = "false"; next; }

# Ignore special doxygen blocks
/^#ifndef DOXYGEN_SKIP/          { doxygen_skip = "next"; next; }
/^#ifdef DOXYGEN_SKIP/           { doxygen_skip = "true"; next; }
/^#endif \/\* DOXYGEN_SKIP \*\// { doxygen_skip = "false"; next; }
/^#else/                         {
	if (doxygen_skip == "next") {
		doxygen_skip = "true";
	} else {
		doxygen_skip = "false";
	}
	next;
}
{ if (doxygen_skip == "true") next }

# Ignore functions that shouldn't be exported to squirrel
/^#ifndef EXPORT_SKIP/          { export_skip = "true"; next; }
/^#endif \/\* EXPORT_SKIP \*\// { export_skip = "false"; next; }
{ if (export_skip == "true") next }

# Ignore the comments
/^#/             { next; }
/\/\*.*\*\//     { comment = "false"; next; }
/\/\*/           { comment = "true";  next; }
/\*\//           { comment = "false"; next; }
{ if (comment == "true") next }

# We need to make specialized conversions for structs
/^(	*)struct/ {
	cls_level++
	if (public == "false") next
	if (cls_level != 1) next
	struct_size++
	structs[struct_size] = cls "::" $2
	next
}

# We need to make specialized conversions for enums
/^(	*)enum/ {
	cls_level++
	if (public == "false") next;
	in_enum = "true"
	enum_size++
	enums[enum_size] = cls "::" $2
	next
}

# Maybe the end of the class, if so we can start with the Squirrel export pretty soon
/};/ {
	cls_level--
	if (cls_level != 0) {
		in_enum = "false";
		next;
	}
	if (cls == "") {
		next;
	}
	start_squirrel_define_on_next_line = "true"
	next;
}

# Empty/white lines. When we may do the Squirrel export, do that export.
/^([ 	]*)$/ {
	if (start_squirrel_define_on_next_line == "false") next
	spaces = "                                                               ";
	public = "false"
	namespace_opened = "false"

	print ""

	# First check whether we have enums to print
	if (enum_size != 0) {
		if (namespace_opened == "false") {
			print "namespace SQConvert {"
			namespace_opened = "true"
		}
		print "	/* Allow enums to be used as Squirrel parameters */"
		for (i = 1; i <= enum_size; i++) {
			print "	template <> " enums[i] " GetParam(ForceType<" enums[i] ">, HSQUIRRELVM vm, int index, SQAutoFreePointers *ptr);"
			print "	template <> int Return<" enums[i] ">(HSQUIRRELVM vm, " enums[i] " res);"
			delete enums[i]
		}
	}

	# Then check whether we have structs/classes to print
	if (struct_size != 0) {
		if (namespace_opened == "false") {
			print "namespace SQConvert {"
			namespace_opened = "true"
		}
		print "	/* Allow inner classes/structs to be used as Squirrel parameters */"
		for (i = 1; i <= struct_size; i++) {
			dump_class_templates(structs[i])
			delete structs[i]
		}
	}

	if (namespace_opened == "false") {
		print "namespace SQConvert {"
		namespace_opened = "true"
	} else {
		print ""
	}
	print "	/* Allow " cls " to be used as Squirrel parameter */"
	dump_class_templates(cls)

	print "}; // namespace SQConvert"

	print "";
	# Then do the registration functions of the class. */
	print "void SQ" cls "_Register(Squirrel *engine);"

	enum_size = 0
	enum_value_size = 0
	enum_string_to_error_size = 0
	enum_error_to_string_size = 0
	struct_size = 0
	method_size = 0
	static_method_size = 0
	virtual_class = "false"
	cls = ""
	start_squirrel_define_on_next_line = "false"
	cls_level = 0
}

# Skip non-public functions
{ if (public == "false") next }

# Add enums
{
	if (in_enum == "true") {
		enum_value_size++
		sub(",", "", $1)
		enum_value[enum_value_size] = $1

		# Check if this a special error enum
		if (match(enums[enum_size], ".*::ErrorMessages") != 0) {
			# syntax:
			# enum ErrorMessages {
			#	ERR_SOME_ERROR,	// [STR_ITEM1, STR_ITEM2, ...]
			# }

			# Set the mappings
			if (match($0, "\\[.*\\]") != 0) {
				mappings = substr($0, RSTART, RLENGTH);
				gsub("([\\[[:space:]\\]])", "", mappings);

				split(mappings, mapitems, ",");
				for (i = 1; i <= length(mapitems); i++) {
					enum_string_to_error_size++
					enum_string_to_error_mapping_string[enum_string_to_error_size] = mapitems[i]
					enum_string_to_error_mapping_error[enum_string_to_error_size] = $1
				}

				enum_error_to_string_size++
				enum_error_to_string_mapping[enum_error_to_string_size] = $1
			}
		}
		next
	}
}

# Add a method to the list
/^.*\(.*\).*$/ {
	if (cls_level != 1) next
	if (match($0, "~")) next

	is_static = match($0, "static")
	if (match($0, "virtual")) {
		virtual_class = "true"
	}
	gsub("virtual", "", $0)
	gsub("static", "", $0)
	gsub("const", "", $0)
	gsub("{.*", "", $0)
	param_s = $0
	gsub("\\*", "", $0)
	gsub("\\(.*", "", $0)

	sub(".*\\(", "", param_s)
	sub("\\).*", "", param_s)

	funcname = $2
	if ($1 == cls && funcname == "") {
		cls_param[0] = param_s
		if (param_s == "") next
	} else if (funcname == "") next

	split(param_s, params, ",")
	if (is_static) {
		types = "?"
	} else {
		types = "x"
	}
	for (len = 1; params[len] != ""; len++) {
		sub("^[ 	]*", "", params[len])
		if (match(params[len], "\\*") || match(params[len], "&")) {
			if (match(params[len], "^char")) {
				types = types "s"
			} else if (match(params[len], "^void")) {
				types = types "p"
			} else if (match(params[len], "^Array")) {
				types = types "a"
			} else if (match(params[len], "^struct Array")) {
				types = types "a"
			} else {
				types = types "x"
			}
		} else if (match(params[len], "^bool")) {
			types = types "b"
		} else if (match(params[len], "^HSQUIRRELVM")) {
			types = "v"
		} else {
			types = types "i"
		}
	}

	if ($1 == cls && funcname == "") {
		cls_param[1] = len;
		cls_param[2] = types;
	} else if (substr(funcname, 0, 1) == "_" && types != "v") {
	} else if (funcname == "GetClassName" && types == "?") {
	} else if (is_static) {
		static_method_size++
		static_methods[static_method_size, 0] = funcname
		static_methods[static_method_size, 1] = len
		static_methods[static_method_size, 2] = types
	} else {
		method_size++
		methods[method_size, 0] = funcname
		methods[method_size, 1] = len
		methods[method_size, 2] = types
	}
	next
}
