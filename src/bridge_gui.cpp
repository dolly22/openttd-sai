/* $Id: bridge_gui.cpp 22702 2011-07-30 17:48:23Z rubidium $ */

/*
 * This file is part of OpenTTD.
 * OpenTTD is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 2.
 * OpenTTD is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details. You should have received a copy of the GNU General Public License along with OpenTTD. If not, see <http://www.gnu.org/licenses/>.
 */

/** @file bridge_gui.cpp Graphical user interface for bridge construction */

#include "stdafx.h"
#include "gui.h"
#include "command_func.h"
#include "rail.h"
#include "strings_func.h"
#include "window_func.h"
#include "sound_func.h"
#include "gfx_func.h"
#include "tunnelbridge.h"
#include "sortlist_type.h"
#include "widgets/dropdown_func.h"
#include "core/geometry_func.hpp"
#include "cmd_helper.h"
#include "tunnelbridge_map.h"
#include "road_gui.h"

#include "table/strings.h"

/** The type of the last built rail bridge */
static BridgeType _last_railbridge_type = 0;
/** The type of the last built road bridge */
static BridgeType _last_roadbridge_type = 0;

/**
 * Carriage for the data we need if we want to build a bridge
 */
struct BuildBridgeData {
	BridgeType index;
	const BridgeSpec *spec;
	Money cost;
};

typedef GUIList<BuildBridgeData> GUIBridgeList; ///< List of bridges, used in #BuildBridgeWindow.

/**
 * Callback executed after a build Bridge CMD has been called
 *
 * @param result Whether the build succeeded
 * @param end_tile End tile of the bridge.
 * @param p1 packed start tile coords (~ dx)
 * @param p2 various bitstuffed elements
 * - p2 = (bit  0- 7) - bridge type (hi bh)
 * - p2 = (bit  8-11) - rail type or road types.
 * - p2 = (bit 15-16) - transport type.
 */
void CcBuildBridge(const CommandCost &result, TileIndex end_tile, uint32 p1, uint32 p2)
{
	if (result.Failed()) return;
	SndPlayTileFx(SND_27_BLACKSMITH_ANVIL, end_tile);

	TransportType transport_type = Extract<TransportType, 15, 2>(p2);

	if (transport_type == TRANSPORT_ROAD) {
		DiagDirection end_direction = ReverseDiagDir(GetTunnelBridgeDirection(end_tile));
		ConnectRoadToStructure(end_tile, end_direction);

		DiagDirection start_direction = ReverseDiagDir(GetTunnelBridgeDirection(p1));
		ConnectRoadToStructure(p1, start_direction);
	}
}

/** Names of the widgets of the build-bridge selection window. */
enum BuildBridgeSelectionWidgets {
	BBSW_CAPTION,
	BBSW_DROPDOWN_ORDER,
	BBSW_DROPDOWN_CRITERIA,
	BBSW_BRIDGE_LIST,
	BBSW_SCROLLBAR,
};

/** Window class for handling the bridge-build GUI. */
class BuildBridgeWindow : public Window {
private:
	/* Runtime saved values */
	static uint16 last_size;     ///< Last size of the bridge GUI window.
	static Listing last_sorting; ///< Last setting of the sort.

	/* Constants for sorting the bridges */
	static const StringID sorter_names[];
	static GUIBridgeList::SortFunction * const sorter_funcs[];

	/* Internal variables */
	TileIndex start_tile;
	TileIndex end_tile;
	uint32 type;
	GUIBridgeList *bridges;
	int bridgetext_offset; ///< Horizontal offset of the text describing the bridge properties in #BBSW_BRIDGE_LIST relative to the left edge.
	Scrollbar *vscroll;

	/** Sort the bridges by their index */
	static int CDECL BridgeIndexSorter(const BuildBridgeData *a, const BuildBridgeData *b)
	{
		return a->index - b->index;
	}

	/** Sort the bridges by their price */
	static int CDECL BridgePriceSorter(const BuildBridgeData *a, const BuildBridgeData *b)
	{
		return a->cost - b->cost;
	}

	/** Sort the bridges by their maximum speed */
	static int CDECL BridgeSpeedSorter(const BuildBridgeData *a, const BuildBridgeData *b)
	{
		return a->spec->speed - b->spec->speed;
	}

	void BuildBridge(uint8 i)
	{
		switch ((TransportType)(this->type >> 15)) {
			case TRANSPORT_RAIL: _last_railbridge_type = this->bridges->Get(i)->index; break;
			case TRANSPORT_ROAD: _last_roadbridge_type = this->bridges->Get(i)->index; break;
			default: break;
		}
		DoCommandP(this->end_tile, this->start_tile, this->type | this->bridges->Get(i)->index,
					CMD_BUILD_BRIDGE | CMD_MSG(STR_ERROR_CAN_T_BUILD_BRIDGE_HERE), CcBuildBridge);
	}

	/** Sort the builable bridges */
	void SortBridgeList()
	{
		this->bridges->Sort();

		/* Display the current sort variant */
		this->GetWidget<NWidgetCore>(BBSW_DROPDOWN_CRITERIA)->widget_data = this->sorter_names[this->bridges->SortType()];

		/* Set the modified widgets dirty */
		this->SetWidgetDirty(BBSW_DROPDOWN_CRITERIA);
		this->SetWidgetDirty(BBSW_BRIDGE_LIST);
	}

public:
	BuildBridgeWindow(const WindowDesc *desc, TileIndex start, TileIndex end, uint32 br_type, GUIBridgeList *bl) : Window(),
		start_tile(start),
		end_tile(end),
		type(br_type),
		bridges(bl)
	{
		this->CreateNestedTree(desc);
		this->vscroll = this->GetScrollbar(BBSW_SCROLLBAR);
		/* Change the data, or the caption of the gui. Set it to road or rail, accordingly. */
		this->GetWidget<NWidgetCore>(BBSW_CAPTION)->widget_data = (GB(this->type, 15, 2) == TRANSPORT_ROAD) ? STR_SELECT_ROAD_BRIDGE_CAPTION : STR_SELECT_RAIL_BRIDGE_CAPTION;
		this->FinishInitNested(desc, GB(br_type, 15, 2)); // Initializes 'this->bridgetext_offset'.

		this->parent = FindWindowById(WC_BUILD_TOOLBAR, GB(this->type, 15, 2));
		this->bridges->SetListing(this->last_sorting);
		this->bridges->SetSortFuncs(this->sorter_funcs);
		this->bridges->NeedResort();
		this->SortBridgeList();

		this->vscroll->SetCount(bl->Length());
		if (this->last_size < this->vscroll->GetCapacity()) this->last_size = this->vscroll->GetCapacity();
		if (this->last_size > this->vscroll->GetCount()) this->last_size = this->vscroll->GetCount();
		/* Resize the bridge selection window if we used a bigger one the last time. */
		if (this->last_size > this->vscroll->GetCapacity()) {
			ResizeWindow(this, 0, (this->last_size - this->vscroll->GetCapacity()) * this->resize.step_height);
		}
		this->GetWidget<NWidgetCore>(BBSW_BRIDGE_LIST)->widget_data = (this->vscroll->GetCapacity() << MAT_ROW_START) + (1 << MAT_COL_START);
	}

	~BuildBridgeWindow()
	{
		this->last_sorting = this->bridges->GetListing();

		delete bridges;
	}

	virtual void UpdateWidgetSize(int widget, Dimension *size, const Dimension &padding, Dimension *fill, Dimension *resize)
	{
		switch (widget) {
			case BBSW_DROPDOWN_ORDER: {
				Dimension d = GetStringBoundingBox(this->GetWidget<NWidgetCore>(widget)->widget_data);
				d.width += padding.width + WD_SORTBUTTON_ARROW_WIDTH * 2; // Doubled since the string is centred and it also looks better.
				d.height += padding.height;
				*size = maxdim(*size, d);
				break;
			}
			case BBSW_DROPDOWN_CRITERIA: {
				Dimension d = {0, 0};
				for (const StringID *str = this->sorter_names; *str != INVALID_STRING_ID; str++) {
					d = maxdim(d, GetStringBoundingBox(*str));
				}
				d.width += padding.width;
				d.height += padding.height;
				*size = maxdim(*size, d);
				break;
			}
			case BBSW_BRIDGE_LIST: {
				Dimension sprite_dim = {0, 0}; // Biggest bridge sprite dimension
				Dimension text_dim   = {0, 0}; // Biggest text dimension
				for (int i = 0; i < (int)this->bridges->Length(); i++) {
					const BridgeSpec *b = this->bridges->Get(i)->spec;
					sprite_dim = maxdim(sprite_dim, GetSpriteSize(b->sprite));

					SetDParam(2, this->bridges->Get(i)->cost);
					SetDParam(1, b->speed);
					SetDParam(0, b->material);
					text_dim = maxdim(text_dim, GetStringBoundingBox(_game_mode == GM_EDITOR ? STR_SELECT_BRIDGE_SCENEDIT_INFO : STR_SELECT_BRIDGE_INFO));
				}
				sprite_dim.height++; // Sprite is rendered one pixel down in the matrix field.
				text_dim.height++; // Allowing the bottom row pixels to be rendered on the edge of the matrix field.
				resize->height = max(sprite_dim.height, text_dim.height) + 2; // Max of both sizes + account for matrix edges.

				this->bridgetext_offset = WD_MATRIX_LEFT + sprite_dim.width + 1; // Left edge of text, 1 pixel distance from the sprite.
				size->width = this->bridgetext_offset + text_dim.width + WD_MATRIX_RIGHT;
				size->height = 4 * resize->height; // Smallest bridge gui is 4 entries high in the matrix.
				break;
			}
		}
	}

	virtual Point OnInitialPosition(const WindowDesc *desc, int16 sm_width, int16 sm_height, int window_number)
	{
		/* Position the window so hopefully the first bridge from the list is under the mouse pointer. */
		NWidgetBase *list = this->GetWidget<NWidgetBase>(BBSW_BRIDGE_LIST);
		Point corner; // point of the top left corner of the window.
		corner.y = Clamp(_cursor.pos.y - list->pos_y - 5, GetMainViewTop(), GetMainViewBottom() - sm_height);
		corner.x = Clamp(_cursor.pos.x - list->pos_x - 5, 0, _screen.width - sm_width);
		return corner;
	}

	virtual void DrawWidget(const Rect &r, int widget) const
	{
		switch (widget) {
			case BBSW_DROPDOWN_ORDER:
				this->DrawSortButtonState(widget, this->bridges->IsDescSortOrder() ? SBS_DOWN : SBS_UP);
				break;

			case BBSW_BRIDGE_LIST: {
				uint y = r.top;
				for (int i = this->vscroll->GetPosition(); this->vscroll->IsVisible(i) && i < (int)this->bridges->Length(); i++) {
					const BridgeSpec *b = this->bridges->Get(i)->spec;

					SetDParam(2, this->bridges->Get(i)->cost);
					SetDParam(1, b->speed);
					SetDParam(0, b->material);

					DrawSprite(b->sprite, b->pal, r.left + WD_MATRIX_LEFT, y + this->resize.step_height - 1 - GetSpriteSize(b->sprite).height);
					DrawStringMultiLine(r.left + this->bridgetext_offset, r.right, y + 2, y + this->resize.step_height,
							_game_mode == GM_EDITOR ? STR_SELECT_BRIDGE_SCENEDIT_INFO : STR_SELECT_BRIDGE_INFO);
					y += this->resize.step_height;
				}
				break;
			}
		}
	}

	virtual EventState OnKeyPress(uint16 key, uint16 keycode)
	{
		const uint8 i = keycode - '1';
		if (i < 9 && i < this->bridges->Length()) {
			/* Build the requested bridge */
			this->BuildBridge(i);
			delete this;
			return ES_HANDLED;
		}
		return ES_NOT_HANDLED;
	}

	virtual void OnClick(Point pt, int widget, int click_count)
	{
		switch (widget) {
			default: break;
			case BBSW_BRIDGE_LIST: {
				uint i = this->vscroll->GetScrolledRowFromWidget(pt.y, this, BBSW_BRIDGE_LIST);
				if (i < this->bridges->Length()) {
					this->BuildBridge(i);
					delete this;
				}
				break;
			}

			case BBSW_DROPDOWN_ORDER:
				this->bridges->ToggleSortOrder();
				this->SetDirty();
				break;

			case BBSW_DROPDOWN_CRITERIA:
				ShowDropDownMenu(this, this->sorter_names, this->bridges->SortType(), BBSW_DROPDOWN_CRITERIA, 0, 0);
				break;
		}
	}

	virtual void OnDropdownSelect(int widget, int index)
	{
		if (widget == BBSW_DROPDOWN_CRITERIA && this->bridges->SortType() != index) {
			this->bridges->SetSortType(index);

			this->SortBridgeList();
		}
	}

	virtual void OnResize()
	{
		this->vscroll->SetCapacityFromWidget(this, BBSW_BRIDGE_LIST);
		this->GetWidget<NWidgetCore>(BBSW_BRIDGE_LIST)->widget_data = (this->vscroll->GetCapacity() << MAT_ROW_START) + (1 << MAT_COL_START);

		this->last_size = max(this->vscroll->GetCapacity(), this->last_size);
	}
};

/** Set the default size of the Build Bridge Window. */
uint16 BuildBridgeWindow::last_size = 4;
/** Set the default sorting for the bridges */
Listing BuildBridgeWindow::last_sorting = {true, 2};

/** Available bridge sorting functions. */
GUIBridgeList::SortFunction * const BuildBridgeWindow::sorter_funcs[] = {
	&BridgeIndexSorter,
	&BridgePriceSorter,
	&BridgeSpeedSorter
};

/** Names of the sorting functions. */
const StringID BuildBridgeWindow::sorter_names[] = {
	STR_SORT_BY_NUMBER,
	STR_SORT_BY_COST,
	STR_SORT_BY_MAX_SPEED,
	INVALID_STRING_ID
};

/** Widgets of the bridge gui. */
static const NWidgetPart _nested_build_bridge_widgets[] = {
	/* Header */
	NWidget(NWID_HORIZONTAL),
		NWidget(WWT_CLOSEBOX, COLOUR_DARK_GREEN),
		NWidget(WWT_CAPTION, COLOUR_DARK_GREEN, BBSW_CAPTION), SetDataTip(STR_SELECT_RAIL_BRIDGE_CAPTION, STR_TOOLTIP_WINDOW_TITLE_DRAG_THIS),
	EndContainer(),

	NWidget(NWID_HORIZONTAL),
		NWidget(NWID_VERTICAL),
			/* Sort order + criteria buttons */
			NWidget(NWID_HORIZONTAL),
				NWidget(WWT_TEXTBTN, COLOUR_DARK_GREEN, BBSW_DROPDOWN_ORDER), SetFill(1, 0), SetDataTip(STR_BUTTON_SORT_BY, STR_TOOLTIP_SORT_ORDER),
				NWidget(WWT_DROPDOWN, COLOUR_DARK_GREEN, BBSW_DROPDOWN_CRITERIA), SetFill(1, 0), SetDataTip(0x0, STR_TOOLTIP_SORT_CRITERIA),
			EndContainer(),
			/* Matrix. */
			NWidget(WWT_MATRIX, COLOUR_DARK_GREEN, BBSW_BRIDGE_LIST), SetFill(1, 0), SetResize(0, 22), SetDataTip(0x401, STR_SELECT_BRIDGE_SELECTION_TOOLTIP), SetScrollbar(BBSW_SCROLLBAR),
		EndContainer(),

		/* scrollbar + resize button */
		NWidget(NWID_VERTICAL),
			NWidget(NWID_VSCROLLBAR, COLOUR_DARK_GREEN, BBSW_SCROLLBAR),
			NWidget(WWT_RESIZEBOX, COLOUR_DARK_GREEN),
		EndContainer(),
	EndContainer(),
};

/** Window definition for the rail bridge selection window. */
static const WindowDesc _build_bridge_desc(
	WDP_AUTO, 200, 114,
	WC_BUILD_BRIDGE, WC_BUILD_TOOLBAR,
	WDF_CONSTRUCTION,
	_nested_build_bridge_widgets, lengthof(_nested_build_bridge_widgets)
);

/**
 * Prepare the data for the build a bridge window.
 *  If we can't build a bridge under the given conditions
 *  show an error message.
 *
 * @param start The start tile of the bridge
 * @param end The end tile of the bridge
 * @param transport_type The transport type
 * @param road_rail_type The road/rail type
 */
void ShowBuildBridgeWindow(TileIndex start, TileIndex end, TransportType transport_type, byte road_rail_type)
{
	DeleteWindowByClass(WC_BUILD_BRIDGE);

	/* Data type for the bridge.
	 * Bit 16,15 = transport type,
	 *     14..8 = road/rail types,
	 *      7..0 = type of bridge */
	uint32 type = (transport_type << 15) | (road_rail_type << 8);

	/* The bridge length without ramps. */
	const uint bridge_len = GetTunnelBridgeLength(start, end);

	/* If Ctrl is being pressed, check wether the last bridge built is available
	 * If so, return this bridge type. Otherwise continue normally.
	 * We store bridge types for each transport type, so we have to check for
	 * the transport type beforehand.
	 */
	BridgeType last_bridge_type = 0;
	switch (transport_type) {
		case TRANSPORT_ROAD: last_bridge_type = _last_roadbridge_type; break;
		case TRANSPORT_RAIL: last_bridge_type = _last_railbridge_type; break;
		default: break; // water ways and air routes don't have bridge types
	}
	if (_ctrl_pressed && CheckBridgeAvailability(last_bridge_type, bridge_len).Succeeded()) {
		DoCommandP(end, start, type | last_bridge_type, CMD_BUILD_BRIDGE | CMD_MSG(STR_ERROR_CAN_T_BUILD_BRIDGE_HERE), CcBuildBridge);
		return;
	}

	/* only query bridge building possibility once, result is the same for all bridges!
	 * returns CMD_ERROR on failure, and price on success */
	StringID errmsg = INVALID_STRING_ID;
	CommandCost ret = DoCommand(end, start, type, CommandFlagsToDCFlags(GetCommandFlags(CMD_BUILD_BRIDGE)) | DC_QUERY_COST, CMD_BUILD_BRIDGE);

	GUIBridgeList *bl = NULL;
	if (ret.Failed()) {
		errmsg = ret.GetErrorMessage();
	} else {
		/* check which bridges can be built */
		const uint tot_bridgedata_len = CalcBridgeLenCostFactor(bridge_len + 2);

		bl = new GUIBridgeList();

		Money infra_cost = 0;
		switch (transport_type) {
			case TRANSPORT_ROAD:
				infra_cost = (bridge_len + 2) * _price[PR_BUILD_ROAD] * 2;
				/* In case we add a new road type as well, we must be aware of those costs. */
				if (IsBridgeTile(start)) infra_cost *= CountBits(GetRoadTypes(start) | (RoadTypes)road_rail_type);
				break;
			case TRANSPORT_RAIL: infra_cost = (bridge_len + 2) * RailBuildCost((RailType)road_rail_type); break;
			default: break;
		}

		/* loop for all bridgetypes */
		for (BridgeType brd_type = 0; brd_type != MAX_BRIDGES; brd_type++) {
			if (CheckBridgeAvailability(brd_type, bridge_len).Succeeded()) {
				/* bridge is accepted, add to list */
				BuildBridgeData *item = bl->Append();
				item->index = brd_type;
				item->spec = GetBridgeSpec(brd_type);
				/* Add to terraforming & bulldozing costs the cost of the
				 * bridge itself (not computed with DC_QUERY_COST) */
				item->cost = ret.GetCost() + (((int64)tot_bridgedata_len * _price[PR_BUILD_BRIDGE] * item->spec->price) >> 8) + infra_cost;
			}
		}
	}

	if (bl != NULL && bl->Length() != 0) {
		new BuildBridgeWindow(&_build_bridge_desc, start, end, type, bl);
	} else {
		delete bl;
		ShowErrorMessage(STR_ERROR_CAN_T_BUILD_BRIDGE_HERE, errmsg, WL_INFO, TileX(end) * TILE_SIZE, TileY(end) * TILE_SIZE);
	}
}
