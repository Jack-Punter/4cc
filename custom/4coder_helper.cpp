/*
 * Miscellaneous helpers for common operations.
 */

// TOP

#define scope_attachment(app,S,I,T) ((T*)managed_scope_get_attachment((app), (S), (I), sizeof(T)))

////////////////////////////////

internal Binding_Unit*
write_unit(Bind_Helper *helper, Binding_Unit unit){
    Binding_Unit *p = 0;
    helper->write_total += sizeof(*p);
    if (helper->error == 0 && helper->cursor != helper->end){
        p = helper->cursor++;
        *p = unit;
    }
    return p;
}

internal Bind_Helper
begin_bind_helper(void *data, i32 size){
    Bind_Helper result = {};
    result.cursor = (Binding_Unit*)data;
    result.start = result.cursor;
    result.end = result.start + size / sizeof(*result.cursor);
    Binding_Unit unit = {};
    unit.type = unit_header;
    unit.header.total_size = sizeof(*result.header);
    result.header = write_unit(&result, unit);
    result.header->header.user_map_count = 0;
    return(result);
}

internal void
begin_map(Bind_Helper *helper, i32 mapid, b32 replace){
    if (helper->group != 0 && helper->error == 0){
        helper->error = BH_ERR_MISSING_END;
    }
    if (!helper->error && mapid < mapid_global){
        ++helper->header->header.user_map_count;
    }
    
    Binding_Unit unit;
    unit.type = unit_map_begin;
    unit.map_begin.mapid = mapid;
    unit.map_begin.replace = replace;
    helper->group = write_unit(helper, unit);
    helper->group->map_begin.bind_count = 0;
}

internal void
begin_map(Bind_Helper *helper, i32 mapid){
    begin_map(helper, mapid, false);
}

internal void
restart_map(Bind_Helper *helper, i32 mapid){
    begin_map(helper, mapid, true);
}

internal void
end_map(Bind_Helper *helper){
    if (helper->group == 0 && helper->error == 0){
        helper->error = BH_ERR_MISSING_BEGIN;
    }
    helper->group = 0;
}

internal void
bind(Bind_Helper *helper, Key_Code code, uint8_t modifiers, Custom_Command_Function *func){
    if (helper->group == 0 && helper->error == 0){
        helper->error = BH_ERR_MISSING_BEGIN;
    }
    if (!helper->error){
        ++helper->group->map_begin.bind_count;
    }
    
    Binding_Unit unit;
    unit.type = unit_callback;
    unit.callback.func = func;
    unit.callback.code = code;
    unit.callback.modifiers = modifiers;
    
    write_unit(helper, unit);
}

internal void
bind(Bind_Helper *helper, Key_Code code, uint8_t modifiers, Generic_Command cmd){
    bind(helper, code, modifiers, cmd.command);
}

internal void
bind_vanilla_keys(Bind_Helper *helper, Custom_Command_Function *func){
    bind(helper, 0, 0, func);
}

internal void
bind_vanilla_keys(Bind_Helper *helper, unsigned char modifiers, Custom_Command_Function *func){
    bind(helper, 0, modifiers, func);
}

internal void
inherit_map(Bind_Helper *helper, i32 mapid){
    if (helper->group == 0 && helper->error == 0) helper->error = BH_ERR_MISSING_BEGIN;
    if (!helper->error && mapid < mapid_global) ++helper->header->header.user_map_count;
    Binding_Unit unit = {};
    unit.type = unit_inherit;
    unit.map_inherit.mapid = mapid;
    write_unit(helper, unit);
}

internal void
set_hook(Bind_Helper *helper, i32 hook_id, Hook_Function *func){
    Binding_Unit unit = {};
    unit.type = unit_hook;
    unit.hook.hook_id = hook_id;
    unit.hook.func = (void*)func;
    write_unit(helper, unit);
}

internal void
set_scroll_rule(Bind_Helper *helper, Delta_Rule_Function *func){
    Binding_Unit unit = {};
    unit.type = unit_hook;
    unit.hook.hook_id = special_hook_scroll_rule;
    unit.hook.func = (void*)func;
    write_unit(helper, unit);
}

internal void
set_buffer_name_resolver(Bind_Helper *helper, Buffer_Name_Resolver_Function *func){
    Binding_Unit unit = {};
    unit.type = unit_hook;
    unit.hook.hook_id = special_hook_buffer_name_resolver;
    unit.hook.func = (void*)func;
    write_unit(helper, unit);
}

internal void
set_modify_color_table_hook(Bind_Helper *helper, Modify_Color_Table_Function *func){
    Binding_Unit unit = {};
    unit.type = unit_hook;
    unit.hook.hook_id = special_hook_modify_color_table;
    unit.hook.func = (void*)func;
    write_unit(helper, unit);
}

internal void
set_clipboard_change_hook(Bind_Helper *helper, Clipboard_Change_Hook_Function *func){
    Binding_Unit unit = {};
    unit.type = unit_hook;
    unit.hook.hook_id = special_hook_clipboard_change;
    unit.hook.func = (void*)func;
    write_unit(helper, unit);
}

internal void
set_get_view_buffer_region_hook(Bind_Helper *helper, Get_View_Buffer_Region_Function *func){
    Binding_Unit unit = {};
    unit.type = unit_hook;
    unit.hook.hook_id = special_hook_get_view_buffer_region;
    unit.hook.func = (void*)func;
    write_unit(helper, unit);
}

internal void
set_new_file_hook(Bind_Helper *helper, Buffer_Hook_Function *func){
    Binding_Unit unit = {};
    unit.type = unit_hook;
    unit.hook.hook_id = special_hook_new_file;
    unit.hook.func = (void*)func;
    write_unit(helper, unit);
}

internal void
set_start_hook(Bind_Helper *helper, Start_Hook_Function *func){
    Binding_Unit unit = {};
    unit.type = unit_hook;
    unit.hook.hook_id = special_hook_start;
    unit.hook.func = (void*)func;
    write_unit(helper, unit);
}

internal void
set_open_file_hook(Bind_Helper *helper, Buffer_Hook_Function *func){
    Binding_Unit unit = {};
    unit.type = unit_hook;
    unit.hook.hook_id = special_hook_open_file;
    unit.hook.func = (void*)func;
    write_unit(helper, unit);
}

internal void
set_save_file_hook(Bind_Helper *helper, Buffer_Hook_Function *func){
    Binding_Unit unit = {};
    unit.type = unit_hook;
    unit.hook.hook_id = special_hook_save_file;
    unit.hook.func = (void*)func;
    write_unit(helper, unit);
}

internal void
set_end_file_hook(Bind_Helper *helper, Buffer_Hook_Function *func){
    Binding_Unit unit = {};
    unit.type = unit_hook;
    unit.hook.hook_id = special_hook_end_file;
    unit.hook.func = (void*)func;
    write_unit(helper, unit);
}

internal void
set_file_edit_range_hook(Bind_Helper *helper, File_Edit_Range_Function *func){
    Binding_Unit unit = {};
    unit.type = unit_hook;
    unit.hook.hook_id = special_hook_file_edit_range;
    unit.hook.func = (void*)func;
    write_unit(helper, unit);
}

internal void
set_file_externally_modified_hook(Bind_Helper *helper, File_Externally_Modified_Function *func){
    Binding_Unit unit = {};
    unit.type = unit_hook;
    unit.hook.hook_id = special_hook_file_externally_modified;
    unit.hook.func = (void*)func;
    write_unit(helper, unit);
}

internal void
set_command_caller(Bind_Helper *helper, Command_Caller_Hook_Function *func){
    Binding_Unit unit = {};
    unit.type = unit_hook;
    unit.hook.hook_id = special_hook_command_caller;
    unit.hook.func = (void*)func;
    write_unit(helper, unit);
}

internal void
set_render_caller(Bind_Helper *helper, Render_Caller_Function *func){
    Binding_Unit unit = {};
    unit.type = unit_hook;
    unit.hook.hook_id = special_hook_render_caller;
    unit.hook.func = (void*)func;
    write_unit(helper, unit);
}

internal void
set_input_filter(Bind_Helper *helper, Input_Filter_Function *func){
    Binding_Unit unit = {};
    unit.type = unit_hook;
    unit.hook.hook_id = special_hook_input_filter;
    unit.hook.func = (void*)func;
    write_unit(helper, unit);
}

internal i32
end_bind_helper(Bind_Helper *helper){
    if (helper->header){
        helper->header->header.total_size = (i32)(helper->cursor - helper->start);
        helper->header->header.error = helper->error;
    }
    i32 result = helper->write_total;
    return(result);
}

internal Bind_Buffer
end_bind_helper_get_buffer(Bind_Helper *helper){
    i32 size = end_bind_helper(helper);
    Bind_Buffer result = {};
    result.data = helper->start;
    result.size = size;
    return(result);
}

internal u32
get_key_code(char *buffer){
    String_Const_u8 str = SCu8(buffer);
    Character_Consume_Result consume = utf8_consume(str.str, str.size);
    return(consume.codepoint);
}

////////////////////////////////

internal Token_Array
get_token_array_from_buffer(Application_Links *app, Buffer_ID buffer){
    Token_Array result = {};
    Managed_Scope scope = buffer_get_managed_scope(app, buffer);
    Token_Array *ptr = scope_attachment(app, scope, attachment_tokens, Token_Array);
    if (ptr != 0){
        result = *ptr;
    }
    return(result);
}

////////////////////////////////

internal Buffer_Seek
seek_location(ID_Line_Column_Jump_Location location){
    return(seek_line_col(location.line, location.column));
}

internal Buffer_Seek
seek_location(ID_Pos_Jump_Location location){
    return(seek_pos(location.pos));
}

internal Buffer_Seek
seek_location(Name_Line_Column_Location location){
    return(seek_line_col(location.line, location.column));
}

internal Buffer_Seek
seek_jump(Parsed_Jump jump){
    return(seek_location(jump.location));
}

////////////////////////////////

internal Character_Predicate
character_predicate_from_function(Character_Predicate_Function *func){
    Character_Predicate predicate = {};
    i32 byte_index = 0;
    for (u32 i = 0; i <= 255;){
        b8 v[8];
        for (i32 bit_index = 0; bit_index < 8; i += 1, bit_index += 1){
            v[bit_index] = func((u8)i);
        }
        predicate.b[byte_index] = (
            (v[0] << 0) |
            (v[1] << 1) |
            (v[2] << 2) |
            (v[3] << 3) |
            (v[4] << 4) |
            (v[5] << 5) |
            (v[6] << 6) |
            (v[7] << 7)
            );
        byte_index += 1;
    }
    return(predicate);
}

internal Character_Predicate
character_predicate_from_character(u8 character){
    Character_Predicate predicate = {};
    predicate.b[character/8] = (1 << (character%8));
    return(predicate);
}

#define character_predicate_check_character(p, c) (((p).b[(c)/8] & (1 << ((c)%8))) != 0)

internal Character_Predicate
character_predicate_or(Character_Predicate *a, Character_Predicate *b){
    Character_Predicate p = {};
    for (i32 i = 0; i < ArrayCount(p.b); i += 1){
        p.b[i] = a->b[i] | b->b[i];
    }
    return(p);
}

internal Character_Predicate
character_predicate_and(Character_Predicate *a, Character_Predicate *b){
    Character_Predicate p = {};
    for (i32 i = 0; i < ArrayCount(p.b); i += 1){
        p.b[i] = a->b[i] & b->b[i];
    }
    return(p);
}

internal Character_Predicate
character_predicate_not(Character_Predicate *a){
    Character_Predicate p = {};
    for (i32 i = 0; i < ArrayCount(p.b); i += 1){
        p.b[i] = ~(a->b[i]);
    }
    return(p);
}

internal i64
buffer_seek_character_class_change__inner(Application_Links *app, Buffer_ID buffer, Character_Predicate *positive, Character_Predicate *negative, Scan_Direction direction, i64 start_pos){
    i64 pos = start_pos;
    switch (direction){
        case Scan_Backward:
        {
            String_Match m1 = buffer_seek_character_class(app, buffer, negative, direction, pos);
            String_Match m2 = buffer_seek_character_class(app, buffer, positive, direction, m1.range.min);
            pos = m2.range.min;
            if (m1.buffer == buffer && m2.buffer == buffer){
                pos += 1;
            }
        }break;
        case Scan_Forward:
        {
            pos -= 1;
            String_Match m1 = buffer_seek_character_class(app, buffer, positive, direction, pos);
            String_Match m2 = buffer_seek_character_class(app, buffer, negative, direction, m1.range.min);
            pos = m2.range.min;
        }break;
    }
    return(pos);
}

internal i64
buffer_seek_character_class_change_1_0(Application_Links *app, Buffer_ID buffer, Character_Predicate *predicate, Scan_Direction direction, i64 start_pos){
    Character_Predicate negative = character_predicate_not(predicate);
    return(buffer_seek_character_class_change__inner(app, buffer, predicate, &negative, direction, start_pos));
}

internal i64
buffer_seek_character_class_change_0_1(Application_Links *app, Buffer_ID buffer, Character_Predicate *predicate, Scan_Direction direction, i64 start_pos){
    Character_Predicate negative = character_predicate_not(predicate);
    return(buffer_seek_character_class_change__inner(app, buffer, &negative, predicate, direction, start_pos));
}

////////////////////////////////

internal i64
view_pos_from_xy(Application_Links *app, View_ID view, Vec2_f32 p){
    Buffer_ID buffer = view_get_buffer(app, view, AccessProtected);
    Rect_f32 region = view_get_buffer_region(app, view);
    f32 width = rect_width(region);
    Face_ID face_id = get_face_id(app, buffer);
    Buffer_Scroll scroll_vars = view_get_buffer_scroll(app, view);
    i64 line = scroll_vars.position.line_number;
    p = (p - region.p0) + scroll_vars.position.pixel_shift;
    return(buffer_pos_at_relative_xy(app, buffer, width, face_id, line, p));
}

internal Buffer_Point
view_move_buffer_point(Application_Links *app, View_ID view, Buffer_Point buffer_point, Vec2_f32 delta){
    delta += buffer_point.pixel_shift;
    Line_Shift_Vertical shift = view_line_shift_y(app, view, buffer_point.line_number, delta.y);
    buffer_point.line_number = shift.line;
    buffer_point.pixel_shift = V2f32(delta.x, delta.y - shift.y_delta);
    return(buffer_point);
}

internal void
view_zero_scroll(Application_Links *app, View_ID view){
    if (view_is_in_ui_mode(app, view)){
        Basic_Scroll scroll = {};
        view_set_basic_scroll(app, view, scroll);
    }
    else{
        Buffer_Scroll scroll = {};
        view_set_buffer_scroll(app, view, scroll);
    }
}

internal void
view_set_cursor_and_preferred_x(Application_Links *app, View_ID view, Buffer_Seek seek){
    view_set_cursor(app, view, seek);
    Buffer_Cursor cursor = view_compute_cursor(app, view, seek);
    Vec2_f32 p = view_relative_xy_of_pos(app, view, cursor.line, cursor.pos);
    view_set_preferred_x(app, view, p.x);
}

////////////////////////////////

internal Range_i64
buffer_range(Application_Links *app, Buffer_ID buffer){
    Range_i64 range = {};
    range.end = buffer_get_size(app, buffer);
    return(range);
}

internal i64
buffer_side(Application_Links *app, Buffer_ID buffer, Side side){
    return(range_side(buffer_range(app, buffer), side));
}

internal Range_i64
get_view_range(Application_Links *app, View_ID view){
    return(Ii64(view_get_cursor_pos(app, view), view_get_mark_pos(app, view)));
}

internal void
set_view_range(Application_Links *app, View_ID view, Range_i64 range){
    i64 c = view_get_cursor_pos(app, view);
    i64 m = view_get_mark_pos(app, view);
    if (c < m){
        view_set_cursor_and_preferred_x(app, view, seek_pos(range.min));
        view_set_mark(app, view, seek_pos(range.max));
    }
    else{
        view_set_mark(app, view, seek_pos(range.min));
        view_set_cursor_and_preferred_x(app, view, seek_pos(range.max));
    }
}

internal b32
is_valid_line(Application_Links *app, Buffer_ID buffer_id, i64 line){
    i64 max_line = buffer_get_line_count(app, buffer_id);
    return(1 <= line && line <= max_line);
}

internal b32
is_valid_line_range(Application_Links *app, Buffer_ID buffer_id, Range_i64 range){
    i64 max_line = buffer_get_line_count(app, buffer_id);
    return(1 <= range.first && range.first <= range.end && range.end <= max_line);
}

internal i64
get_line_number_from_pos(Application_Links *app, Buffer_ID buffer, i64 pos){
    Buffer_Cursor cursor = buffer_compute_cursor(app, buffer, seek_pos(pos));
    return(cursor.line);
}

internal i64
buffer_get_character_legal_pos_from_pos(Application_Links *app, Buffer_ID buffer, f32 width, Face_ID face, i64 pos){
    Buffer_Cursor cursor = buffer_compute_cursor(app, buffer, seek_pos(pos));
    i64 character = buffer_relative_character_from_pos(app, buffer, width, face, cursor.line, pos);
    return(buffer_pos_from_relative_character(app, buffer, width, face, cursor.line, character));
}

internal i64
view_get_character_legal_pos_from_pos(Application_Links *app, Buffer_ID buffer, i64 pos){
    Buffer_Cursor cursor = buffer_compute_cursor(app, buffer, seek_pos(pos));
    i64 character = view_relative_character_from_pos(app, buffer, cursor.line, pos);
    return(view_pos_from_relative_character(app, buffer, cursor.line, character));
}

internal Buffer_Cursor
get_line_side(Application_Links *app, Buffer_ID buffer, i64 line_number, Side side){
    i64 character_index = (side == Side_Min)?(1):(-1);
    return(buffer_compute_cursor(app, buffer, seek_line_col(line_number, character_index)));
}
internal i64
get_line_side_pos(Application_Links *app, Buffer_ID buffer, i64 line_number, Side side){
    i64 pos = -1;
    Buffer_Cursor cursor = get_line_side(app, buffer, line_number, side);
    if (cursor.line != 0){
        pos = cursor.pos;
    }
    return(pos);
}

internal Buffer_Cursor
get_line_start(Application_Links *app, Buffer_ID buffer, i64 line_number){
    return(get_line_side(app, buffer, line_number, Side_Min));
}
internal i64
get_line_start_pos(Application_Links *app, Buffer_ID buffer, i64 line_number){
    return(get_line_side_pos(app, buffer, line_number, Side_Min));
}

// NOTE(allen): The position returned has the index of the terminating newline character,
// not one past the newline character.
internal Buffer_Cursor
get_line_end(Application_Links *app, Buffer_ID buffer, i64 line_number){
    return(get_line_side(app, buffer, line_number, Side_Max));
}
internal i64
get_line_end_pos(Application_Links *app, Buffer_ID buffer, i64 line_number){
    return(get_line_side_pos(app, buffer, line_number, Side_Max));
}

// NOTE(allen): The range returned does not include the terminating newline character
internal Range_Cursor
get_line_range(Application_Links *app, Buffer_ID buffer, i64 line_number){
    b32 success = false;
    Range_Cursor result = {};
    result.begin = get_line_start(app, buffer, line_number);
    if (result.begin.line != 0){
        result.end = get_line_end(app, buffer, line_number);
        if (result.end.line != 0){
            success = true;
        }
    }
    if (!success){
        block_zero_struct(&result);
    }
    return(result);
}

// NOTE(allen): The range returned does not include the terminating newline character
internal Range_i64
get_line_pos_range(Application_Links *app, Buffer_ID buffer, i64 line_number){
    Range_Cursor range = get_line_range(app, buffer, line_number);
    Range_i64 result = {};
    if (range.begin.line != 0 && range.end.line != 0){
        result = Ii64(range.begin.pos, range.end.pos);
    }
    return(result);
}

internal Range_i64
make_range_from_cursors(Range_Cursor range){
    return(Ii64(range.begin.pos, range.end.pos));
}

internal i64
get_line_side_pos_from_pos(Application_Links *app, Buffer_ID buffer, i64 pos, Side side){
    i64 line_number = get_line_number_from_pos(app, buffer, pos);
    return(get_line_side_pos(app, buffer, line_number, side));
}
internal i64
get_line_start_pos_from_pos(Application_Links *app, Buffer_ID buffer, i64 pos){
    return(get_line_side_pos_from_pos(app, buffer, pos, Side_Min));
}
internal i64
get_line_end_pos_from_pos(Application_Links *app, Buffer_ID buffer, i64 pos){
    return(get_line_side_pos_from_pos(app, buffer, pos, Side_Max));
}

internal Token*
get_first_token_from_line(Application_Links *app, Buffer_ID buffer, Token_Array tokens, i64 line){
    i64 line_start = get_line_start_pos(app, buffer, line);
    return(token_from_pos(&tokens, line_start));
}

////////////////////////////////

internal i64
scan_any_boundary(Application_Links *app, Boundary_Function *func, Buffer_ID buffer, Scan_Direction direction, i64 pos){
    i64 a = func(app, buffer, Side_Min, direction, pos);
    i64 b = func(app, buffer, Side_Max, direction, pos);
    i64 result = 0;
    if (direction == Scan_Forward){
        result = Min(a, b);
    }
    else{
        result = Max(a, b);
    }
    return(result);
}

internal i64
scan(Application_Links *app, Boundary_Function *func, Buffer_ID buffer, Scan_Direction direction, i64 pos){
    Side side = (direction == Scan_Forward)?(Side_Max):(Side_Min);
    return(func(app, buffer, side, direction, pos));
}

internal i64
scan(Application_Links *app, Boundary_Function_List funcs, Buffer_ID buffer, Scan_Direction direction, i64 start_pos){
    i64 result = 0;
    if (direction == Scan_Forward){
        i64 size = buffer_get_size(app, buffer);
        result = size + 1;
        for (Boundary_Function_Node *node = funcs.first;
             node != 0;
             node = node->next){
            i64 pos = scan(app, node->func, buffer, direction, start_pos);
            result = Min(result, pos);
        }
    }
    else{
        result = -1;
        for (Boundary_Function_Node *node = funcs.first;
             node != 0;
             node = node->next){
            i64 pos = scan(app, node->func, buffer, direction, start_pos);
            result = Max(result, pos);
        }
    }
    return(result);
}

internal void
push_boundary(Arena *arena, Boundary_Function_List *list, Boundary_Function *func){
    Boundary_Function_Node *node = push_array(arena, Boundary_Function_Node, 1);
    sll_queue_push(list->first, list->last, node);
    list->count += 1;
    node->func = func;
}

internal Boundary_Function_List
push_boundary_list__innerv(Arena *arena, va_list args){
    Boundary_Function_List list = {};
    for (;;){
        Boundary_Function *func = va_arg(args, Boundary_Function*);
        if (func == 0){
            break;
        }
        push_boundary(arena, &list, func);
    }
    return(list);
}
internal Boundary_Function_List
push_boundary_list__inner(Arena *arena, ...){
    va_list args;
    va_start(args, arena);
    Boundary_Function_List result = push_boundary_list__innerv(arena, args);
    va_end(args);
    return(result);
}
#define push_boundary_list(a,...) push_boundary_list__inner((a), __VA_ARGS__, 0)

internal i64
boundary_predicate(Application_Links *app, Buffer_ID buffer, Side side, Scan_Direction direction, i64 pos, Character_Predicate *predicate){
    i64 result = 0;
    switch (side){
        case Side_Min:
        {
            result = buffer_seek_character_class_change_0_1(app, buffer, predicate, direction, pos);
        }break;
        case Side_Max:
        {
            result = buffer_seek_character_class_change_1_0(app, buffer, predicate, direction, pos);
        }break;
    }
    return(result);
}

internal i64
boundary_non_whitespace(Application_Links *app, Buffer_ID buffer, Side side, Scan_Direction direction, i64 pos){
    return(boundary_predicate(app, buffer, side, direction, pos, &character_predicate_non_whitespace));
}

internal i64
boundary_base10(Application_Links *app, Buffer_ID buffer, Side side, Scan_Direction direction, i64 pos){
    return(boundary_predicate(app, buffer, side, direction, pos, &character_predicate_base10));
}

internal i64
boundary_base10_colon(Application_Links *app, Buffer_ID buffer, Side side, Scan_Direction direction, i64 pos){
    internal Character_Predicate predicate = {};
    internal b32 first_call = true;
    if (first_call){
        first_call = false;
        Character_Predicate colon = character_predicate_from_character((u8)':');
        predicate = character_predicate_or(&character_predicate_base10, &colon);
    }
    return(boundary_predicate(app, buffer, side, direction, pos, &predicate));
}

internal i64
boundary_base16(Application_Links *app, Buffer_ID buffer, Side side, Scan_Direction direction, i64 pos){
    return(boundary_predicate(app, buffer, side, direction, pos, &character_predicate_base16));
}

internal i64
boundary_alpha_numeric(Application_Links *app, Buffer_ID buffer, Side side, Scan_Direction direction, i64 pos){
    return(boundary_predicate(app, buffer, side, direction, pos, &character_predicate_alpha_numeric));
}

internal i64
boundary_alpha_numeric_unicode(Application_Links *app, Buffer_ID buffer, Side side, Scan_Direction direction, i64 pos){
    return(boundary_predicate(app, buffer, side, direction, pos, &character_predicate_alpha_numeric_underscore_utf8));
}

internal i64
boundary_alpha_numeric_underscore(Application_Links *app, Buffer_ID buffer, Side side, Scan_Direction direction, i64 pos){
    return(boundary_predicate(app, buffer, side, direction, pos, &character_predicate_alpha_numeric_underscore));
}

internal i64
boundary_alpha_numeric_underscore_utf8(Application_Links *app, Buffer_ID buffer, Side side, Scan_Direction direction, i64 pos){
    return(boundary_predicate(app, buffer, side, direction, pos, &character_predicate_alpha_numeric_underscore_utf8));
}

internal i64
boundary_alpha_numeric_camel(Application_Links *app, Buffer_ID buffer, Side side, Scan_Direction direction, i64 pos){
    i64 an_pos = boundary_alpha_numeric(app, buffer, side, direction, pos);
    String_Match m = buffer_seek_character_class(app, buffer, &character_predicate_uppercase, direction, pos);
    i64 cap_pos = m.range.min;
    if (side == Side_Max){
        i64 an_left_pos = boundary_alpha_numeric(app, buffer, flip_side(side), flip_direction(direction), an_pos);
        if (cap_pos == an_left_pos){
            m = buffer_seek_character_class(app, buffer, &character_predicate_uppercase, direction, cap_pos);
            cap_pos = m.range.min;
        }
    }
    i64 result = 0;
    if (direction == Scan_Backward){
        result = Max(an_pos, cap_pos);
    }
    else{
        result = Min(an_pos, cap_pos);
    }
    return(result);
}

internal i64
boundary_inside_quotes(Application_Links *app, Buffer_ID buffer, Side side, Scan_Direction direction, i64 pos){
    internal Character_Predicate predicate = {};
    internal b32 first_call = true;
    if (first_call){
        first_call = false;
        predicate = character_predicate_from_character((u8)'"');
        predicate = character_predicate_not(&predicate);
    }
    return(boundary_predicate(app, buffer, side, direction, pos, &predicate));
}

internal i64
boundary_token(Application_Links *app, Buffer_ID buffer, Side side, Scan_Direction direction, i64 pos){
    i64 result = boundary_non_whitespace(app, buffer, side, direction, pos);
    Token_Array tokens = get_token_array_from_buffer(app, buffer);
    if (tokens.tokens != 0){
        switch (direction){
            case Scan_Forward:
            {
                i64 buffer_size = buffer_get_size(app, buffer);
                result = buffer_size;
                if (tokens.count > 0){
                    Token_Iterator_Array it = token_iterator_pos(0, &tokens, pos);
                    Token *token = token_it_read(&it);
                    if (token->kind == TokenBaseKind_Whitespace){
                        token_it_inc_non_whitespace(&it);
                        token = token_it_read(&it);
                    }
                    if (token != 0){
                        if (side == Side_Max){
                            result = token->pos + token->size;
                        }
                        else{
                            if (token->pos <= pos){
                                token_it_inc_non_whitespace(&it);
                                token = token_it_read(&it);
                            }
                            if (token != 0){
                                result = token->pos;
                            }
                        }
                    }
                }
            }break;
            
            case Scan_Backward:
            {
                result = 0;
                if (tokens.count > 0){
                    Token_Iterator_Array it = token_iterator_pos(0, &tokens, pos);
                    Token *token = token_it_read(&it);
                    if (token->kind == TokenBaseKind_Whitespace){
                        token_it_dec_non_whitespace(&it);
                        token = token_it_read(&it);
                    }
                    if (token != 0){
                        if (side == Side_Min){
                            if (token->pos >= pos){
                                token_it_dec_non_whitespace(&it);
                                token = token_it_read(&it);
                            }
                            result = token->pos;
                        }
                        else{
                            if (token->pos + token->size >= pos){
                                token_it_dec_non_whitespace(&it);
                                token = token_it_read(&it);
                            }
                            result = token->pos + token->size;
                        }
                    }
                }
            }break;
        }
    }
    return(result);
}

internal i64
boundary_line(Application_Links *app, Buffer_ID buffer, Side side, Scan_Direction direction, i64 pos){
    i64 line_number = get_line_number_from_pos(app, buffer, pos);
    i64 new_pos = get_line_side_pos(app, buffer, line_number, side);
    if (direction == Scan_Backward && new_pos >= pos){
        if (line_number > 1){
            new_pos = get_line_side_pos(app, buffer, line_number - 1, side);
        }
        else{
            new_pos = 0;
        }
    }
    else if (direction == Scan_Forward && new_pos <= pos){
        new_pos = get_line_side_pos(app, buffer, line_number + 1, side);
        if (new_pos <= pos){
            new_pos = (i32)buffer_get_size(app, buffer);
        }
    }
    return(new_pos);
}

////////////////////////////////

// TODO(allen): these need a little more rewrite
internal void
seek_string_forward(Application_Links *app, Buffer_ID buffer, i64 pos, i64 end, String_Const_u8 needle, i64 *result){
    if (end == 0){
        end = (i32)buffer_get_size(app, buffer);
    }
    String_Match match = {};
    match.range.first = pos;
    for (;;){
        match = buffer_seek_string(app, buffer, needle, Scan_Forward, (i32)match.range.first);
        if (HasFlag(match.flags, StringMatch_CaseSensitive) ||
            match.buffer != buffer || match.range.first >= end) break;
    }
    if (match.range.first < end && match.buffer == buffer){
        *result = match.range.first;
    }
    else{
        *result = buffer_get_size(app, buffer);
    }
}

internal void
seek_string_backward(Application_Links *app, Buffer_ID buffer, i64 pos, i64 min, String_Const_u8 needle, i64 *result){
    String_Match match = {};
    match.range.first = pos;
    for (;;){
        match = buffer_seek_string(app, buffer, needle, Scan_Backward, match.range.first);
        if (HasFlag(match.flags, StringMatch_CaseSensitive) ||
            match.buffer != buffer || match.range.first < min) break;
    }
    if (match.range.first >= min && match.buffer == buffer){
        *result = match.range.first;
    }
    else{
        *result = -1;
    }
}

internal void
seek_string_insensitive_forward(Application_Links *app, Buffer_ID buffer, i64 pos, i64 end, String_Const_u8 needle, i64 *result){
    if (end == 0){
        end = (i32)buffer_get_size(app, buffer);
    }
    String_Match match = buffer_seek_string(app, buffer, needle, Scan_Forward, pos);
    if (match.range.first < end && match.buffer == buffer){
        *result = match.range.first;
    }
    else{
        *result = buffer_get_size(app, buffer);
    }
}

internal void
seek_string_insensitive_backward(Application_Links *app, Buffer_ID buffer, i64 pos, i64 min, String_Const_u8 needle, i64 *result){
    String_Match match = buffer_seek_string(app, buffer, needle, Scan_Backward, pos);
    if (match.range.first >= min && match.buffer == buffer){
        *result = match.range.first;
    }
    else{
        *result = -1;
    }
}

internal void
seek_string(Application_Links *app, Buffer_ID buffer_id, i64 pos, i64 end, i64 min, String_Const_u8 str, i64 *result, Buffer_Seek_String_Flags flags){
    switch (flags & 3){
        case 0:
        {
            seek_string_forward(app, buffer_id, pos, end, str, result);
        }break;
        
        case BufferSeekString_Backward:
        {
            seek_string_backward(app, buffer_id, pos, min, str, result);
        }break;
        
        case BufferSeekString_CaseInsensitive:
        {
            seek_string_insensitive_forward(app, buffer_id, pos, end, str, result);
        }break;
        
        case BufferSeekString_Backward|BufferSeekString_CaseInsensitive:
        {
            seek_string_insensitive_backward(app, buffer_id, pos, min, str, result);
        }break;
    }
}

////////////////////////////////

internal Range_i64
get_line_range_from_pos_range(Application_Links *app, Buffer_ID buffer, Range_i64 pos_range){
    Range_i64 line_range = {};
    line_range.first = get_line_number_from_pos(app, buffer, pos_range.first);
    if (line_range.first != 0){
        line_range.end = get_line_number_from_pos(app, buffer, pos_range.one_past_last);
        if (line_range.end == 0){
            line_range.first = 0;
        }
    }
    return(line_range);
}

// NOTE(allen): The end of the returned range does not include the terminating newline character of
// the last line.
internal Range_i64
get_pos_range_from_line_range(Application_Links *app, Buffer_ID buffer, Range_i64 line_range){
    Range_i64 pos_range = {};
    if (is_valid_line_range(app, buffer, line_range)){
        pos_range.first = get_line_start_pos(app, buffer, line_range.first);
        pos_range.one_past_last = get_line_end_pos(app, buffer, line_range.end);
    }
    return(pos_range);
}

internal Range_i64
enclose_boundary(Application_Links *app, Buffer_ID buffer, Range_i64 range, Boundary_Function *func){
    i64 new_min       = func(app, buffer, Side_Min, Scan_Backward, range.min + 1);
    i64 new_min_check = func(app, buffer, Side_Max, Scan_Backward, range.min + 1);
    if (new_min_check <= new_min && new_min < range.min){
        range.min = new_min;
    }
    i64 new_max       = func(app, buffer, Side_Max, Scan_Forward, range.max - 1);
    i64 new_max_check = func(app, buffer, Side_Min, Scan_Forward, range.max - 1);
    if (new_max_check >= new_max && new_max > range.max){
        range.max = new_max;
    }
    return(range);
}

internal Range_i64
enclose_non_whitespace(Application_Links *app, Buffer_ID buffer, Range_i64 range){
    return(enclose_boundary(app, buffer, range, boundary_non_whitespace));
}
internal Range_i64
enclose_pos_non_whitespace(Application_Links *app, Buffer_ID buffer, i64 pos){
    return(enclose_boundary(app, buffer, Ii64(pos), boundary_non_whitespace));
}

internal Range_i64
enclose_tokens(Application_Links *app, Buffer_ID buffer, Range_i64 range){
    return(enclose_boundary(app, buffer, range, boundary_token));
}
internal Range_i64
enclose_pos_tokens(Application_Links *app, Buffer_ID buffer, i64 pos){
    return(enclose_boundary(app, buffer, Ii64(pos), boundary_token));
}

internal Range_i64
enclose_base10(Application_Links *app, Buffer_ID buffer, Range_i64 range){
    return(enclose_boundary(app, buffer, range, boundary_base10));
}
internal Range_i64
enclose_pos_base10(Application_Links *app, Buffer_ID buffer, i64 pos){
    return(enclose_boundary(app, buffer, Ii64(pos), boundary_base10));
}

internal Range_i64
enclose_base16(Application_Links *app, Buffer_ID buffer, Range_i64 range){
    return(enclose_boundary(app, buffer, range, boundary_base16));
}
internal Range_i64
enclose_pos_base16(Application_Links *app, Buffer_ID buffer, i64 pos){
    return(enclose_boundary(app, buffer, Ii64(pos), boundary_base16));
}

internal Range_i64
enclose_base10_colon(Application_Links *app, Buffer_ID buffer, Range_i64 range){
    return(enclose_boundary(app, buffer, range, boundary_base10_colon));
}
internal Range_i64
enclose_pos_base10_colon(Application_Links *app, Buffer_ID buffer, i64 pos){
    return(enclose_boundary(app, buffer, Ii64(pos), boundary_base10_colon));
}

internal Range_i64
enclose_alpha_numeric(Application_Links *app, Buffer_ID buffer, Range_i64 range){
    return(enclose_boundary(app, buffer, range, boundary_alpha_numeric));
}
internal Range_i64
enclose_pos_alpha_numeric(Application_Links *app, Buffer_ID buffer, i64 pos){
    return(enclose_boundary(app, buffer, Ii64(pos), boundary_alpha_numeric));
}

internal Range_i64
enclose_alpha_numeric_unicode(Application_Links *app, Buffer_ID buffer, Range_i64 range){
    return(enclose_boundary(app, buffer, range, boundary_alpha_numeric_unicode));
}
internal Range_i64
enclose_pos_alpha_numeric_unicode(Application_Links *app, Buffer_ID buffer, i64 pos){
    return(enclose_boundary(app, buffer, Ii64(pos), boundary_alpha_numeric_unicode));
}

internal Range_i64
enclose_alpha_numeric_underscore(Application_Links *app, Buffer_ID buffer, Range_i64 range){
    return(enclose_boundary(app, buffer, range, boundary_alpha_numeric_underscore));
}
internal Range_i64
enclose_pos_alpha_numeric_underscore(Application_Links *app, Buffer_ID buffer, i64 pos){
    return(enclose_boundary(app, buffer, Ii64(pos), boundary_alpha_numeric_underscore));
}

internal Range_i64
enclose_alpha_numeric_underscore_utf8(Application_Links *app, Buffer_ID buffer, Range_i64 range){
    return(enclose_boundary(app, buffer, range, boundary_alpha_numeric_underscore_utf8));
}
internal Range_i64
enclose_pos_alpha_numeric_underscore_utf8(Application_Links *app, Buffer_ID buffer, i64 pos){
    return(enclose_boundary(app, buffer, Ii64(pos), boundary_alpha_numeric_underscore_utf8));
}

internal Range_i64
enclose_alpha_numeric_camel(Application_Links *app, Buffer_ID buffer, Range_i64 range){
    return(enclose_boundary(app, buffer, range, boundary_alpha_numeric_camel));
}
internal Range_i64
enclose_pos_alpha_numeric_camel(Application_Links *app, Buffer_ID buffer, i64 pos){
    return(enclose_boundary(app, buffer, Ii64(pos), boundary_alpha_numeric_camel));
}

internal Range_i64
enclose_pos_inside_quotes(Application_Links *app, Buffer_ID buffer, i64 pos){
    return(enclose_boundary(app, buffer, Ii64(pos), boundary_inside_quotes));
}

internal Range_i64
enclose_whole_lines(Application_Links *app, Buffer_ID buffer, Range_i64 range){
    return(enclose_boundary(app, buffer, range, boundary_line));
}
internal Range_i64
enclose_pos_whole_lines(Application_Links *app, Buffer_ID buffer, i64 pos){
    return(enclose_boundary(app, buffer, Ii64(pos), boundary_line));
}

////////////////////////////////

internal Range_i64
get_snipe_range(Application_Links *app, Boundary_Function_List funcs, Buffer_ID buffer, i64 pos, Scan_Direction direction){
    Range_i64 result = {};
    i64 buffer_size = buffer_get_size(app, buffer);
    i64 pos0 = pos;
    i64 pos1 = scan(app, funcs, buffer, direction, pos0);
    if (0 <= pos1 && pos1 <= buffer_size){
        i64 pos2 = scan(app, funcs, buffer, flip_direction(direction), pos1);
        if (0 <= pos2 && pos2 <= buffer_size){
            if (direction == Scan_Backward){
                pos2 = clamp_bot(pos2, pos0);
            }
            else{
                pos2 = clamp_top(pos2, pos0);
            }
            result = Ii64(pos1, pos2);
        }
    }
    return(result);
}

internal Range_i64
get_snipe_range(Application_Links *app, Boundary_Function *func, Buffer_ID buffer, i64 pos, Scan_Direction direction){
    Scratch_Block scratch(app);
    return(get_snipe_range(app, push_boundary_list(scratch, func), buffer, pos, direction));
}

////////////////////////////////

internal String_Const_u8
push_buffer_range(Application_Links *app, Arena *arena, Buffer_ID buffer, Range_i64 range){
    String_Const_u8 result = {};
    i64 length = range_size(range);
    if (length > 0){
        Temp_Memory restore_point = begin_temp(arena);
        u8 *memory = push_array(arena, u8, length);
        if (buffer_read_range(app, buffer, range, (char*)memory)){
            result = SCu8(memory, length);
        }
        else{
            end_temp(restore_point);
        }
    }
    return(result);
}

internal String_Const_u8
push_token_lexeme(Application_Links *app, Arena *arena, Buffer_ID buffer, Token token){
    return(push_buffer_range(app, arena, buffer, Ii64(token.pos, token.pos + token.size)));
}

internal String_Const_u8
push_buffer_line(Application_Links *app, Arena *arena, Buffer_ID buffer, i64 line_number){
    return(push_buffer_range(app, arena, buffer, get_line_pos_range(app, buffer, line_number)));
}

internal String_Const_u8
push_whole_buffer(Application_Links *app, Arena *arena, Buffer_ID buffer){
    return(push_buffer_range(app, arena, buffer, buffer_range(app, buffer)));
}

internal String_Const_u8
push_view_range_string(Application_Links *app, Arena *arena, View_ID view){
    Buffer_ID buffer = view_get_buffer(app, view, AccessProtected);
    return(push_buffer_range(app, arena, buffer, get_view_range(app, view)));
}

internal String_Const_u8
push_view_range_string(Application_Links *app, Arena *arena){
    View_ID view = get_active_view(app, AccessAll);
    return(push_view_range_string(app, arena, view));
}

internal String_Const_u8
push_enclose_range_at_pos(Application_Links *app, Arena *arena, Buffer_ID buffer, i64 pos, Enclose_Function *enclose){
    Range_i64 range = enclose(app, buffer, Ii64(pos));
    return(push_buffer_range(app, arena, buffer, range));
}

////////////////////////////////

internal b32
buffer_has_name_with_star(Application_Links *app, Buffer_ID buffer){
    Scratch_Block scratch(app);
    String_Const_u8 str = push_buffer_unique_name(app, scratch, buffer);
    return(str.size > 0 && str.str[0] == '*');
}

internal char
buffer_get_char(Application_Links *app, Buffer_ID buffer_id, i64 pos){
    i64 buffer_size = buffer_get_size(app, buffer_id);
    char result = ' ';
    if (0 <= pos && pos < buffer_size){
        buffer_read_range(app, buffer_id, Ii64(pos, pos + 1), &result);
    }
    return(result);
}

internal b32
token_lexeme_string_match(Application_Links *app, Buffer_ID buffer, Token token, String_Const_u8 b){
    Scratch_Block scratch(app);
    return(string_match(push_token_lexeme(app, scratch, buffer, token), b));
}

internal b32
line_is_valid_and_blank(Application_Links *app, Buffer_ID buffer, i64 line_number){
    b32 result = false;
    if (is_valid_line(app, buffer, line_number)){
        Scratch_Block scratch(app);
        String_Const_u8 line = push_buffer_line(app, scratch, buffer, line_number);
        result = true;
        for (u64 i = 0; i < line.size; i += 1){
            if (!character_is_whitespace(line.str[i])){
                result = false;
                break;
            }
        }
    }
    return(result);
}

////////////////////////////////

internal i64
get_pos_past_lead_whitespace_from_line_number(Application_Links *app, Buffer_ID buffer, i64 line_number){
    Scratch_Block scratch(app);
    Range_i64 line_range = get_line_pos_range(app, buffer, line_number);
    String_Const_u8 line = push_buffer_range(app, scratch, buffer, line_range);
    i64 result = line_range.end;
    for (u64 i = 0; i < line.size; i += 1){
        if (!character_is_whitespace(line.str[i])){
            result = line_range.start + i;
            break;
        }
    }
    return(result);
}

internal i64
get_pos_past_lead_whitespace(Application_Links *app, Buffer_ID buffer, i64 pos){
    i64 line_number = get_line_number_from_pos(app, buffer, pos);
    i64 result = get_pos_past_lead_whitespace_from_line_number(app, buffer, line_number);
    result = clamp_bot(pos, result);
    return(result);
}

internal void
move_past_lead_whitespace(Application_Links *app, View_ID view, Buffer_ID buffer){
    i64 pos = view_get_cursor_pos(app, view);
    i64 new_pos = get_pos_past_lead_whitespace(app, buffer, pos);
    view_set_cursor_and_preferred_x(app, view, seek_pos(new_pos));
}

internal void
move_past_lead_whitespace(Application_Links *app, View_ID view){
    Buffer_ID buffer = view_get_buffer(app, view, AccessProtected);
    move_past_lead_whitespace(app, view, buffer);
}

internal b32
line_is_blank(Application_Links *app, Buffer_ID buffer, i64 line_number){
    Scratch_Block scratch(app);
    String_Const_u8 line = push_buffer_line(app, scratch, buffer, line_number);
    b32 is_blank = true;
    for (umem i = 0; i < line.size; i += 1){
        if (!character_is_whitespace(line.str[i])){
            is_blank = false;
            break;
        }
    }
    return(is_blank);
}

internal i64
get_line_number_of__whitespace_status_line(Application_Links *app, Buffer_ID buffer, Scan_Direction direction, i64 line_number_start, b32 get_blank_line){
    i64 line_count = buffer_get_line_count(app, buffer);
    i64 line_number = line_number_start + direction;
    for (;1 <= line_number && line_number <= line_count; line_number += direction){
        b32 is_blank = line_is_blank(app, buffer, line_number);
        if (is_blank == get_blank_line){
            break;
        }
    }
    line_number = clamp(1, line_number, line_count);
    return(line_number);
}

internal i64
get_line_number_of_non_blank_line(Application_Links *app, Buffer_ID buffer, Scan_Direction direction, i64 line_number_start){
    return(get_line_number_of__whitespace_status_line(app, buffer, direction, line_number_start, false));
}

internal i64
get_line_number_of_blank_line(Application_Links *app, Buffer_ID buffer, Scan_Direction direction, i64 line_number_start){
    return(get_line_number_of__whitespace_status_line(app, buffer, direction, line_number_start, true));
}

internal i64
get_pos_of_blank_line(Application_Links *app, Buffer_ID buffer, Scan_Direction direction, i64 pos_start){
    i64 line_number_start = get_line_number_from_pos(app, buffer, pos_start);
    i64 blank_line = get_line_number_of_blank_line(app, buffer, direction, line_number_start);
    i64 pos = get_line_start_pos(app, buffer, blank_line);
    return(pos);
}

internal i64
get_line_number_of_blank_line_grouped(Application_Links *app, Buffer_ID buffer, Scan_Direction direction, i64 line_number_start){
    i64 line_number = line_number_start;
    if (line_is_blank(app, buffer, line_number)){
        line_number = get_line_number_of_non_blank_line(app, buffer, direction, line_number);
    }
    line_number = get_line_number_of_blank_line(app, buffer, direction, line_number);
    return(line_number);
}

internal i64
get_pos_of_blank_line_grouped(Application_Links *app, Buffer_ID buffer, Scan_Direction direction, i64 pos_start){
    i64 line_number_start = get_line_number_from_pos(app, buffer, pos_start);
    i64 blank_line = get_line_number_of_blank_line_grouped(app, buffer, direction, line_number_start);
    i64 pos = get_line_start_pos(app, buffer, blank_line);
    return(pos);
}

internal Indent_Info
get_indent_info_range(Application_Links *app, Buffer_ID buffer, Range_i64 range, i32 tab_width){
    Scratch_Block scratch(app);
    String_Const_u8 s = push_buffer_range(app, scratch, buffer, range);
    
    i32 tab_additional_width = tab_width - 1;
    
    Indent_Info info = {};
    info.first_char_pos = range.end;
    info.is_blank = true;
    info.all_space = true;
    for (umem i = 0; i < s.size; i += 1){
        u8 c = s.str[i];
        if (!character_is_whitespace(c)){
            info.is_blank = false;
            info.all_space = false;
            info.first_char_pos = range.start + (i64)i;
            break;
        }
        if (c != ' '){
            info.all_space = false;
        }
        if (c == '\t'){
            info.indent_pos += tab_additional_width;
        }
        info.indent_pos += 1;
    }
    
    return(info);
}

internal Indent_Info
get_indent_info_line_start(Application_Links *app, Buffer_ID buffer, i64 line_start, i32 tab_width){
    i64 end = get_line_side_pos_from_pos(app, buffer, line_start, Side_Max);
    return(get_indent_info_range(app, buffer, Ii64(line_start, end), tab_width));
}

////////////////////////////////

internal History_Group
history_group_begin(Application_Links *app, Buffer_ID buffer){
    History_Group group = {};
    group.app = app;
    group.buffer = buffer;
    group.first = buffer_history_get_current_state_index(app, buffer);
    group.first += 1;
    return(group);
}

internal void
history_group_end(History_Group group){
    History_Record_Index last = buffer_history_get_current_state_index(group.app, group.buffer);
    if (group.first < last){
        buffer_history_merge_record_range(group.app, group.buffer, group.first, last, RecordMergeFlag_StateInRange_MoveStateForward);
    }
}

////////////////////////////////

internal void
replace_in_range(Application_Links *app, Buffer_ID buffer, Range_i64 range, String_Const_u8 needle, String_Const_u8 string){
    // TODO(allen): rewrite
    History_Group group = history_group_begin(app, buffer);
    i64 pos = range.min - 1;
    i64 new_pos = 0;
    seek_string_forward(app, buffer, pos, range.end, needle, &new_pos);
    i64 shift = replace_range_shift(needle.size, string.size);
    for (; new_pos + (imem)needle.size <= range.end;){
        Range_i64 needle_range = Ii64(new_pos, new_pos + (i32)needle.size);
        buffer_replace_range(app, buffer, needle_range, string);
        range.end += shift;
        pos = new_pos + (i32)string.size - 1;
        seek_string_forward(app, buffer, pos, range.end, needle, &new_pos);
    }
    history_group_end(group);
}

internal Range_i64
swap_lines(Application_Links *app, Buffer_ID buffer, i64 line_1, i64 line_2){
    Range_i64 result = {};
    i64 line_count = buffer_get_line_count(app, buffer);
    if (1 <= line_1 && line_2 <= line_count){
        Range_i64 range_1 = get_line_pos_range(app, buffer, line_1);
        Range_i64 range_2 = get_line_pos_range(app, buffer, line_2);
        
        Scratch_Block scratch(app);
        
        String_Const_u8 text_1 = push_buffer_range(app, scratch, buffer, range_1);
        String_Const_u8 text_2 = push_buffer_range(app, scratch, buffer, range_2);
        
        History_Group group = history_group_begin(app, buffer);
        buffer_replace_range(app, buffer, range_2, text_1);
        buffer_replace_range(app, buffer, range_1, text_2);
        history_group_end(group);
        
        i64 shift = replace_range_shift(range_1, text_2.size);
        result.min = range_1.min;
        result.max = range_2.min + shift;
    }
    return(result);
}

internal i64
move_line(Application_Links *app, Buffer_ID buffer, i64 line_number, Scan_Direction direction){
    i64 line_1 = 0;
    i64 line_2 = 0;
    if (direction == Scan_Forward){
        line_1 = line_number;
        line_2 = line_number + 1;
    }
    else{
        line_1 = line_number - 1;
        line_2 = line_number;
    }
    Range_i64 line_starts = swap_lines(app, buffer, line_1, line_2);
    i64 result = 0;
    if (line_starts.min < line_starts.max){
        if (direction == Scan_Forward){
            result = line_starts.max;
        }
        else{
            result = line_starts.min;
        }
    }
    else{
        result = get_line_side_pos(app, buffer, line_number, Side_Min);
    }
    return(result);
}

internal void
clear_buffer(Application_Links *app, Buffer_ID buffer){
    buffer_replace_range(app, buffer, buffer_range(app, buffer), string_u8_litexpr(""));
}

////////////////////////////////

internal String_Match_List
find_all_matches_all_buffers(Application_Links *app, Arena *arena, String_Const_u8_Array match_patterns, String_Match_Flag must_have_flags, String_Match_Flag must_not_have_flags){
    String_Match_List all_matches = {};
    for (Buffer_ID buffer = get_buffer_next(app, 0, AccessAll);
         buffer != 0;
         buffer = get_buffer_next(app, buffer, AccessAll)){
        String_Match_List buffer_matches = {};
        for (i32 i = 0; i < match_patterns.count; i += 1){
            Range_i64 range = buffer_range(app, buffer);
            String_Match_List pattern_matches = buffer_find_all_matches(app, arena, buffer, i, range, match_patterns.vals[i],
                                                                        &character_predicate_alpha_numeric_underscore_utf8, Scan_Forward);
            string_match_list_filter_flags(&pattern_matches, must_have_flags, must_not_have_flags);
            if (pattern_matches.count > 0){
                if (buffer_matches.count == 0){
                    buffer_matches = pattern_matches;
                }
                else{
                    buffer_matches = string_match_list_merge_front_to_back(&buffer_matches, &pattern_matches);
                }
            }
        }
        all_matches = string_match_list_join(&all_matches, &buffer_matches);
    }
    return(all_matches);
}

internal String_Match_List
find_all_matches_all_buffers(Application_Links *app, Arena *arena, String_Const_u8 pattern, String_Match_Flag must_have_flags, String_Match_Flag must_not_have_flags){
    String_Const_u8_Array array = {&pattern, 1};
    return(find_all_matches_all_buffers(app, arena, array, must_have_flags, must_not_have_flags));
}

////////////////////////////////

internal void
exec_command(Application_Links *app, Custom_Command_Function *func){
    func(app);
}

internal void
exec_command(Application_Links *app, Generic_Command cmd){
    exec_command(app, cmd.command);
}

internal b32
key_is_unmodified(Key_Event_Data *key){
    b8 *mods = key->modifiers;
    return(!mods[MDFR_CONTROL_INDEX] && !mods[MDFR_ALT_INDEX]);
}

internal u32
to_writable_character(User_Input in, u8 *space){
    u32 inc = 0;
    if (in.key.character != 0){
        inc = utf8_write(space, in.key.character);
    }
    return(inc);
}

internal u32
to_writable_character(Key_Event_Data key, u8 *space){
    u32 inc = 0;
    if (key.character != 0){
        inc = utf8_write(space, key.character);
    }
    return(inc);
}

internal String_Const_u8
backspace_utf8(String_Const_u8 string){
    if (string.size > 0){
        umem i = string.size - 1;
        for (; i > 0; --i){
            if (string.str[i] <= 0x7F || string.str[i] >= 0xC0){
                break;
            }
        }
        string.size = i;
    }
    return(string);
}

internal b32
query_user_general(Application_Links *app, Query_Bar *bar, b32 force_number){
    // NOTE(allen|a3.4.4): It will not cause an *error* if we continue on after failing to.
    // start a query bar, but it will be unusual behavior from the point of view of the
    // user, if this command starts intercepting input even though no prompt is shown.
    // This will only happen if you have a lot of bars open already or if the current view
    // doesn't support query bars.
    if (start_query_bar(app, bar, 0) == 0){
        return(false);
    }
    
    b32 success = true;
    
    for (;;){
        // NOTE(allen|a3.4.4): This call will block until the user does one of the input
        // types specified in the flags.  The first set of flags are inputs you'd like to intercept
        // that you don't want to abort on.  The second set are inputs that you'd like to cause
        // the command to abort.  If an event satisfies both flags, it is treated as an abort.
        User_Input in = get_user_input(app, EventOnAnyKey, EventOnEsc|EventOnMouseLeftButton|EventOnMouseRightButton);
        
        // NOTE(allen|a3.4.4): The responsible thing to do on abort is to end the command
        // without waiting on get_user_input again.
        if (in.abort){
            success = false;
            break;
        }
        
        u8 character[4];
        u32 length = 0;
        b32 good_character = false;
        if (key_is_unmodified(&in.key)){
            if (force_number){
                if (in.key.character >= '0' && in.key.character <= '9'){
                    good_character = true;
                    length = to_writable_character(in, character);
                }
            }
            else{
                length = to_writable_character(in, character);
                if (length != 0){
                    good_character = true;
                }
            }
        }
        
        // NOTE(allen|a3.4.4): All we have to do to update the query bar is edit our
        // local Query_Bar struct!  This is handy because it means our Query_Bar
        // is always correct for typical use without extra work updating the bar.
        if (in.key.keycode == '\n' || in.key.keycode == '\t'){
            break;
        }
        else if (in.key.keycode == key_back){
            bar->string = backspace_utf8(bar->string);
        }
        else if (good_character){
            String_u8 string = Su8(bar->string.str, bar->string.size, bar->string_capacity);
            string_append(&string, SCu8(character, length));
            bar->string = string.string;
        }
    }
    
    return(success);
}

internal b32
query_user_string(Application_Links *app, Query_Bar *bar){
    return(query_user_general(app, bar, false));
}

internal b32
query_user_number(Application_Links *app, Query_Bar *bar){
    return(query_user_general(app, bar, true));
}

////////////////////////////////

internal Buffer_Identifier
buffer_identifier(char *str, i32 len){
    Buffer_Identifier identifier;
    identifier.name = str;
    identifier.name_len = len;
    identifier.id = 0;
    return(identifier);
}

internal Buffer_Identifier
buffer_identifier(String_Const_u8 str){
    return(buffer_identifier((char*)str.str, (i32)str.size));
}

internal Buffer_Identifier
buffer_identifier(Buffer_ID id){
    Buffer_Identifier identifier;
    identifier.name = 0;
    identifier.name_len = 0;
    identifier.id = id;
    return(identifier);
}

internal Buffer_ID
buffer_identifier_to_id(Application_Links *app, Buffer_Identifier identifier){
    Buffer_ID id = 0;
    if (identifier.id != 0){
        id = identifier.id;
    }
    else{
        String_Const_u8 name = SCu8(identifier.name, identifier.name_len);
        id = get_buffer_by_name(app, name, AccessAll);
        if (id == 0){
            id = get_buffer_by_file_name(app, name, AccessAll);
        }
    }
    return(id);
}

internal Buffer_ID
buffer_identifier_to_id_create_out_buffer(Application_Links *app, Buffer_Identifier buffer_id){
    Buffer_ID result = 0;
    if (buffer_id.name != 0 && buffer_id.name_len > 0){
        String_Const_u8 buffer_name = SCu8(buffer_id.name, buffer_id.name_len);
        Buffer_ID buffer_attach_id = get_buffer_by_name(app, buffer_name, AccessAll);
        if (buffer_attach_id != 0){
            result = buffer_attach_id;
        }
        else{
            buffer_attach_id = create_buffer(app, buffer_name, BufferCreate_AlwaysNew|BufferCreate_NeverAttachToFile);
            if (buffer_attach_id != 0){
                buffer_set_setting(app, buffer_attach_id, BufferSetting_ReadOnly, true);
                buffer_set_setting(app, buffer_attach_id, BufferSetting_Unimportant, true);
                result = buffer_attach_id;
            }
        }
    }
    else{
        result = buffer_id.id;
    }
    return(result);
}

////////////////////////////////

function void
place_begin_and_end_on_own_lines(Application_Links *app, char *begin, char *end){
    View_ID view = get_active_view(app, AccessOpen);
    Buffer_ID buffer = view_get_buffer(app, view, AccessOpen);
    
    Range_i64 range = get_view_range(app, view);
    Range_i64 lines = get_line_range_from_pos_range(app, buffer, range);
    range = get_pos_range_from_line_range(app, buffer, lines);
    
    Scratch_Block scratch(app);
    
    b32 min_line_blank = line_is_valid_and_blank(app, buffer, lines.min);
    b32 max_line_blank = line_is_valid_and_blank(app, buffer, lines.max);
    
    if ((lines.min < lines.max) || (!min_line_blank)){
        String_Const_u8 begin_str = {};
        String_Const_u8 end_str = {};
        
        i64 min_adjustment = 0;
        i64 max_adjustment = 0;
        
        if (min_line_blank){
            begin_str = push_u8_stringf(scratch, "\n%s", begin);
            min_adjustment += 1;
        }
        else{
            begin_str = push_u8_stringf(scratch, "%s\n", begin);
        }
        if (max_line_blank){
            end_str = push_u8_stringf(scratch, "%s\n", end);
        }
        else{
            end_str = push_u8_stringf(scratch, "\n%s", end);
            max_adjustment += 1;
        }
        
        max_adjustment += begin_str.size;
        Range_i64 new_pos = Ii64(range.min + min_adjustment, range.max + max_adjustment);
        
        History_Group group = history_group_begin(app, buffer);
        buffer_replace_range(app, buffer, Ii64(range.min), begin_str);
        buffer_replace_range(app, buffer, Ii64(range.max + begin_str.size), end_str);
        history_group_end(group);
        
        set_view_range(app, view, new_pos);
    }
    else{
        String_Const_u8 str = push_u8_stringf(scratch, "%s\n\n%s", begin, end);
        buffer_replace_range(app, buffer, range, str);
        i64 center_pos = range.min + cstring_length(begin) + 1;
        view_set_cursor_and_preferred_x(app, view, seek_pos(center_pos));
        view_set_mark(app, view, seek_pos(center_pos));
    }
}

////////////////////////////////

internal View_ID
open_view(Application_Links *app, View_ID view_location, View_Split_Position position){
    View_ID result = 0;
    if (view_location != 0 && view_exists(app, view_location)){
        Panel_ID panel_id = view_get_panel(app, view_location);
        if (panel_id != 0){
            b32 vertical = (position == ViewSplit_Left || position == ViewSplit_Right);
            if (panel_split(app, panel_id, vertical?PanelSplit_LeftAndRight:PanelSplit_TopAndBottom)){
                Panel_Child child = (position == ViewSplit_Left || position == ViewSplit_Top)?PanelChild_Min:PanelChild_Max;
                Panel_ID new_panel_id = panel_get_child(app, panel_id, child);
                if (new_panel_id != 0){
                    View_ID new_view_id = panel_get_view(app, new_panel_id);
                    if (new_view_id != 0){
                        result = new_view_id;
                    }
                }
            }
        }
    }
    return(result);
}

internal View_ID
get_first_view_with_buffer(Application_Links *app, Buffer_ID buffer_id){
    View_ID result = {};
    if (buffer_id != 0){
        for (View_ID test = get_view_next(app, 0, AccessAll);
             test != 0;
             test = get_view_next(app, test, AccessAll)){
            Buffer_ID test_buffer = view_get_buffer(app, test, AccessAll);
            if (test_buffer == buffer_id){
                result = test;
                break;
            }
        }
    }
    return(result);
}

internal b32
open_file(Application_Links *app, Buffer_ID *buffer_out, String_Const_u8 file_name, b32 background, b32 never_new){
    b32 result = false;
    Buffer_ID buffer = get_buffer_by_name(app, file_name, AccessProtected);
    b32 exists = buffer_exists(app, buffer);
    if (!exists){
        Buffer_Create_Flag flags = 0;
        if (background){
            flags |= BufferCreate_Background;
        }
        if (never_new){
            flags |= BufferCreate_NeverNew;
        }
        buffer = create_buffer(app, file_name, flags);
        exists = buffer_exists(app, buffer);
    }
    if (exists){
        if (buffer_out != 0){
            *buffer_out = buffer;
        }
        result = true;
    }
    return(result);
}

internal b32
view_open_file(Application_Links *app, View_ID view, String_Const_u8 file_name, b32 never_new){
    b32 result = false;
    if (view != 0){
        Buffer_ID buffer = 0;
        if (open_file(app, &buffer, file_name, false, never_new)){
            view_set_buffer(app, view, buffer, 0);
            result = true;
        }
    }
    return(result);
}

internal void
view_disable_highlight_range(Application_Links *app, View_ID view){
    Managed_Scope scope = view_get_managed_scope(app, view);
    Managed_Object *highlight = scope_attachment(app, scope, view_highlight_range, Managed_Object);
    if (*highlight != 0){
        managed_object_free(app, *highlight);
    }
    managed_scope_attachment_erase(app, scope, view_highlight_range);
    managed_scope_attachment_erase(app, scope, view_highlight_buffer);
}

internal void
view_set_highlight_range(Application_Links *app, View_ID view, Range_i64 range){
    view_disable_highlight_range(app, view);
    
    Buffer_ID buffer = view_get_buffer(app, view, AccessAll);
    Managed_Scope scope = view_get_managed_scope(app, view);
    Managed_Object *highlight = scope_attachment(app, scope, view_highlight_range, Managed_Object);
    *highlight = alloc_buffer_markers_on_buffer(app, buffer, 2, &scope);
    Marker markers[2] = {};
    markers[0].pos = range.min;
    markers[1].pos = range.max;
    managed_object_store_data(app, *highlight, 0, 2, markers);
    Buffer_ID *highlight_buffer = scope_attachment(app, scope, view_highlight_buffer, Buffer_ID);
    *highlight_buffer = buffer;
}

function void
view_look_at_region(Application_Links *app, View_ID view, i64 major_pos, i64 minor_pos){
    Range_i64 range = Ii64(major_pos, minor_pos);
    b32 bottom_major = false;
    if (major_pos == range.max){
        bottom_major = true;
    }
    
    Buffer_Cursor top = view_compute_cursor(app, view, seek_pos(range.min));
    if (top.line > 0){
        Buffer_Cursor bottom = view_compute_cursor(app, view, seek_pos(range.max));
        if (bottom.line > 0){
            Rect_f32 region = view_get_buffer_region(app, view);
            f32 view_height = rect_height(region);
            f32 skirt_height = view_height*.1f;
            Interval_f32 acceptable_y = If32(skirt_height, view_height*.9f);
            
            f32 target_height = view_line_y_difference(app, view, bottom.line, top.line);
            
            if (target_height > view_height){
                i64 major_line = bottom.line;
                if (range.min == major_pos){
                    major_line = top.line;
                }
                
                Buffer_Scroll scroll = view_get_buffer_scroll(app, view);
                scroll.target.line_number = major_line;
                scroll.target.pixel_shift.y = -skirt_height;
                view_set_buffer_scroll(app, view, scroll);
            }
            else{
                Buffer_Scroll scroll = view_get_buffer_scroll(app, view);
                Vec2_f32 top_p = view_relative_xy_of_pos(app, view, scroll.position.line_number, range.min);
                top_p -= scroll.position.pixel_shift;
                if (top_p.y < acceptable_y.min){
                    scroll.target.line_number = top.line;
                    scroll.target.pixel_shift.y = -skirt_height;
                    view_set_buffer_scroll(app, view, scroll);
                }
                else{
                    Vec2_f32 bot_p = view_relative_xy_of_pos(app, view, scroll.position.line_number, range.max);
                    bot_p -= scroll.position.pixel_shift;
                    if (bot_p.y > acceptable_y.max){
                        scroll.target.line_number = bottom.line;
                        scroll.target.pixel_shift.y = skirt_height - view_height;
                        view_set_buffer_scroll(app, view, scroll);
                    }
                }
            }
        }
    }
}

////////////////////////////////

internal View_ID
get_next_view_looped_all_panels(Application_Links *app, View_ID view_id, Access_Flag access){
    view_id = get_view_next(app, view_id, access);
    if (view_id == 0){
        view_id = get_view_next(app, 0, access);
    }
    return(view_id);
}

internal View_ID
get_prev_view_looped_all_panels(Application_Links *app, View_ID view_id, Access_Flag access){
    view_id = get_view_prev(app, view_id, access);
    if (view_id == 0){
        view_id = get_view_prev(app, 0, access);
    }
    return(view_id);
}

////////////////////////////////

internal Buffer_Kill_Result
try_buffer_kill(Application_Links *app, Buffer_ID buffer, View_ID gui_view_id, Buffer_Kill_Flag flags){
    Buffer_Kill_Result result = buffer_kill(app, buffer, flags);
    if (result == BufferKillResult_Dirty){
        do_gui_sure_to_kill(app, buffer, gui_view_id);
    }
    return(result);
}

////////////////////////////////

internal String_Const_u8
get_query_string(Application_Links *app, char *query_str, u8 *string_space, i32 space_size){
    Query_Bar bar;
    bar.prompt = SCu8((u8*)query_str);
    bar.string = SCu8(string_space, (umem)0);
    bar.string_capacity = space_size;
    if (!query_user_string(app, &bar)){
        bar.string.size = 0;
    }
    return(bar.string);
}

internal Token*
get_token_from_pos(Application_Links *app, Token_Array *array, u64 pos){
    Token *result = 0;
    if (array->count > 0){
        i64 index = token_index_from_pos(array, pos);
        result = array->tokens + index;
    }
    return(result);
}

internal Token*
get_token_from_pos(Application_Links *app, Buffer_ID buffer, u64 pos){
    Token_Array array = get_token_array_from_buffer(app, buffer);
    return(get_token_from_pos(app, &array, pos));
}

internal String_Const_u8
push_token_or_word_under_pos(Application_Links *app, Arena *arena, Buffer_ID buffer, u64 pos){
    String_Const_u8 result = {};
    Token *token = get_token_from_pos(app, buffer, pos);
    if (token != 0 && token->size > 0 && token->kind != TokenBaseKind_Whitespace){
        Interval_i64 range = Ii64(token->pos, token->pos + token->size);
        result = push_buffer_range(app, arena, buffer, range);
    }
    return(result);
}

internal String_Const_u8
push_token_or_word_under_active_cursor(Application_Links *app, Arena *arena){
    View_ID view = get_active_view(app, AccessAll);
    Buffer_ID buffer = view_get_buffer(app, view, AccessAll);
    i64 pos = view_get_cursor_pos(app, view);
    return(push_token_or_word_under_pos(app, arena, buffer, pos));
}

////////////////////////////////

internal b32
file_exists(Application_Links *app, String_Const_u8 file_name){
    File_Attributes attributes = get_file_attributes(app, file_name);
    return(attributes.last_write_time > 0);
}

internal b32
file_exists_and_is_file(Application_Links *app, String_Const_u8 file_name){
    File_Attributes attributes = get_file_attributes(app, file_name);
    return(attributes.last_write_time > 0 && !HasFlag(attributes.flags, FileAttribute_IsDirectory));
}

internal b32
file_exists_and_is_folder(Application_Links *app, String_Const_u8 file_name){
    File_Attributes attributes = get_file_attributes(app, file_name);
    return(attributes.last_write_time > 0 && HasFlag(attributes.flags, FileAttribute_IsDirectory));
}

internal Data
dump_file_handle(Arena *arena, FILE *file){
    Data result = {};
    if (file != 0){
        fseek(file, 0, SEEK_END);
        umem size = ftell(file);
        char *mem = push_array(arena, char, size);
        if (mem != 0){
            fseek(file, 0, SEEK_SET);
            fread(mem, 1, size, file);
            result = make_data(mem, size);
        }
    }
    return(result);
}

internal String_Const_u8
push_file_search_up_path(Application_Links *app, Arena *arena, String_Const_u8 start_path, String_Const_u8 file_name){
    String_Const_u8 result = {};
    String_Const_u8 path = start_path;
    for (;path.size > 0;){
        Temp_Memory temp = begin_temp(arena);
        if (character_is_slash(string_get_character(path, path.size - 1))){
            path = string_chop(path, 1);
        }
        String_Const_u8 full_path = push_u8_stringf(arena, "%.*s/%.*s",
                                                    string_expand(path),
                                                    string_expand(file_name));
        if (file_exists(app, full_path)){
            result = full_path;
            break;
        }
        path = string_remove_last_folder(path);
        end_temp(temp);
    }
    return(result);
}

internal FILE*
open_file_try_current_path_then_binary_path(Application_Links *app, char *file_name){
    FILE *file = fopen(file_name, "rb");
    if (file == 0){
        Scratch_Block scratch(app);
        List_String_Const_u8 list = {};
        string_list_push(scratch, &list, system_get_path(scratch, SystemPath_Binary));
        string_list_push_overlap(scratch, &list, '/', SCu8(file_name));
        String_Const_u8 str = string_list_flatten(scratch, list, StringFill_NullTerminate);
        file = fopen((char*)str.str, "rb");
    }
    return(file);
}

internal FILE*
open_file(Arena *scratch, String_Const_u8 name){
    Temp_Memory temp = begin_temp(scratch);
    String_Const_u8 name_copy = push_string_copy(scratch, name);
    FILE *file = fopen((char*)name_copy.str, "rb");
    end_temp(temp);
    return(file);
}

internal File_Name_Data
dump_file(Arena *arena, String_Const_u8 file_name){
    File_Name_Data result = {};
    FILE *file = open_file(arena, file_name);
    if (file != 0){
        result.file_name = file_name;
        result.data = dump_file_handle(arena, file);
        fclose(file);
    }
    return(result);
}

internal File_Name_Data
dump_file_search_up_path(Application_Links *app, Arena *arena, String_Const_u8 path, String_Const_u8 file_name){
    File_Name_Data result = {};
    String_Const_u8 full_path = push_file_search_up_path(app, arena, path, file_name);
    if (full_path.size > 0){
        result = dump_file(arena, full_path);
    }
    return(result);
}

internal void
sort_pairs_by_key__quick(Sort_Pair_i32 *pairs, i32 first, i32 one_past_last){
    i32 dif = one_past_last - first;
    if (dif >= 2){
        i32 pivot = one_past_last - 1;
        Sort_Pair_i32 pivot_pair = pairs[pivot];
        i32 j = first;
        b32 interleave = false;
        for (i32 i = first; i < pivot; i += 1){
            Sort_Pair_i32 pair = pairs[i];
            if (pair.key < pivot_pair.key){
                pairs[i] = pairs[j];
                pairs[j] = pair;
                j += 1;
            }
            else if (pair.key == pivot_pair.key){
                if (interleave){
                    pairs[i] = pairs[j];
                    pairs[j] = pair;
                    j += 1;
                }
                interleave = !interleave;
            }
        }
        pairs[pivot] = pairs[j];
        pairs[j] = pivot_pair;
        sort_pairs_by_key__quick(pairs, first, j);
        sort_pairs_by_key__quick(pairs, j + 1, one_past_last);
    }
}

internal void
sort_pairs_by_key(Sort_Pair_i32 *pairs, i32 count){
    sort_pairs_by_key__quick(pairs, 0, count);
}

internal Range_Array
get_ranges_of_duplicate_keys(Arena *arena, i32 *keys, i32 stride, i32 count){
    Range_Array result = {};
    result.ranges = push_array(arena, Range, count);
    u8 *ptr = (u8*)keys;
    i32 start_i = 0;
    for (i32 i = 1; i <= count; i += 1){
        b32 is_end = false;
        if (i == count){
            is_end = true;
        }
        else if (*(i32*)(ptr + i*stride) != *(i32*)(ptr + start_i*stride)){
            is_end = true;
        }
        if (is_end){
            Range *new_range = &result.ranges[result.count++];
            new_range->first = start_i;
            new_range->one_past_last = i;
            start_i = i;
        }
    }
    pop_array(arena, Range, count - result.count);
    return(result);
}

internal void
no_mark_snap_to_cursor(Application_Links *app, Managed_Scope view_scope){
    b32 *snap_to_cursor = scope_attachment(app, view_scope, view_snap_mark_to_cursor, b32);
    *snap_to_cursor = false;
}

internal void
no_mark_snap_to_cursor(Application_Links *app, View_ID view_id){
    Managed_Scope scope = view_get_managed_scope(app, view_id);
    no_mark_snap_to_cursor(app, scope);
}

internal void
no_mark_snap_to_cursor_if_shift(Application_Links *app, View_ID view_id){
    User_Input in = get_command_input(app);
    if (in.key.modifiers[MDFR_SHIFT_INDEX]){
        no_mark_snap_to_cursor(app, view_id);
    }
}

internal b32
view_has_highlighted_range(Application_Links *app, View_ID view){
    b32 result = false;
    if (fcoder_mode == FCoderMode_NotepadLike){
        i64 pos = view_get_cursor_pos(app, view);
        i64 mark = view_get_mark_pos(app, view);
        result = (pos != mark);
    }
    return(result);
}

internal b32
if_view_has_highlighted_range_delete_range(Application_Links *app, View_ID view_id){
    b32 result = false;
    if (view_has_highlighted_range(app, view_id)){
        Range_i64 range = get_view_range(app, view_id);
        Buffer_ID buffer = view_get_buffer(app, view_id, AccessOpen);
        buffer_replace_range(app, buffer, range, string_u8_litexpr(""));
        result = true;
    }
    return(result);
}

internal void
begin_notepad_mode(Application_Links *app){
    fcoder_mode = FCoderMode_NotepadLike;
    for (View_ID view = get_view_next(app, 0, AccessAll);
         view != 0;
         view = get_view_next(app, view, AccessAll)){
        i64 pos = view_get_cursor_pos(app, view);
        view_set_mark(app, view, seek_pos(pos));
    }
}

////////////////////////////////

internal void
seek_pos_of_textual_line(Application_Links *app, Side side){
    View_ID view = get_active_view(app, AccessProtected);
    Buffer_ID buffer = view_get_buffer(app, view, AccessProtected);
    i64 pos = view_get_cursor_pos(app, view);
    i64 new_pos = get_line_side_pos_from_pos(app, buffer, pos, side);
    view_set_cursor_and_preferred_x(app, view, seek_pos(new_pos));
    no_mark_snap_to_cursor_if_shift(app, view);
}

internal void
seek_pos_of_visual_line(Application_Links *app, Side side){
    View_ID view = get_active_view(app, AccessProtected);
    i64 pos = view_get_cursor_pos(app, view);
    Buffer_Cursor cursor = view_compute_cursor(app, view, seek_pos(pos));
    Vec2_f32 p = view_relative_xy_of_pos(app, view, cursor.line, pos);
    p.x = (side == Side_Min)?(0.f):(max_f32);
    i64 new_pos = view_pos_at_relative_xy(app, view, cursor.line, p);
    view_set_cursor_and_preferred_x(app, view, seek_pos(new_pos));
    no_mark_snap_to_cursor_if_shift(app, view);
}

CUSTOM_COMMAND_SIG(seek_beginning_of_textual_line)
CUSTOM_DOC("Seeks the cursor to the beginning of the line across all text.")
{
    seek_pos_of_textual_line(app, Side_Min);
}

CUSTOM_COMMAND_SIG(seek_end_of_textual_line)
CUSTOM_DOC("Seeks the cursor to the end of the line across all text.")
{
    seek_pos_of_textual_line(app, Side_Max);
}

CUSTOM_COMMAND_SIG(seek_beginning_of_line)
CUSTOM_DOC("Seeks the cursor to the beginning of the visual line.")
{
    seek_pos_of_visual_line(app, Side_Min);
}

CUSTOM_COMMAND_SIG(seek_end_of_line)
CUSTOM_DOC("Seeks the cursor to the end of the visual line.")
{
    seek_pos_of_visual_line(app, Side_Max);
}

CUSTOM_COMMAND_SIG(goto_beginning_of_file)
CUSTOM_DOC("Sets the cursor to the beginning of the file.")
{
    View_ID view = get_active_view(app, AccessProtected);
    view_set_cursor_and_preferred_x(app, view, seek_pos(0));
    no_mark_snap_to_cursor_if_shift(app, view);
}

CUSTOM_COMMAND_SIG(goto_end_of_file)
CUSTOM_DOC("Sets the cursor to the end of the file.")
{
    View_ID view = get_active_view(app, AccessProtected);
    Buffer_ID buffer_id = view_get_buffer(app, view, AccessProtected);
    i32 size = (i32)buffer_get_size(app, buffer_id);
    view_set_cursor_and_preferred_x(app, view, seek_pos(size));
    no_mark_snap_to_cursor_if_shift(app, view);
}

////////////////////////////////

internal b32
view_set_split(Application_Links *app, View_ID view, View_Split_Kind kind, f32 t){
    b32 result = false;
    if (view != 0){
        Panel_ID panel_id = view_get_panel(app, view);
        if (panel_id != 0){
            Panel_ID parent_panel_id = panel_get_parent(app, panel_id);
            if (parent_panel_id != 0){
                Panel_ID min_child_id = panel_get_child(app, parent_panel_id, PanelChild_Min);
                if (min_child_id != 0){
                    b32 panel_is_min = (min_child_id == panel_id);
                    Panel_Split_Kind panel_kind = ((kind == ViewSplitKind_Ratio)?
                                                   (panel_is_min?PanelSplitKind_Ratio_Min:PanelSplitKind_Ratio_Max):
                                                   (panel_is_min?PanelSplitKind_FixedPixels_Min:PanelSplitKind_FixedPixels_Max));
                    result = panel_set_split(app, parent_panel_id, panel_kind, t);
                }
            }
        }
    }
    return(result);
}

internal b32
view_set_split_proportion(Application_Links *app, View_ID view, f32 t){
    return(view_set_split(app, view, ViewSplitKind_Ratio, t));
}

internal b32
view_set_split_pixel_size(Application_Links *app, View_ID view, i32 t){
    return(view_set_split(app, view, ViewSplitKind_FixedPixels, (f32)t));
}

////////////////////////////////

internal Record_Info
get_single_record(Application_Links *app, Buffer_ID buffer_id, History_Record_Index index){
    Record_Info record = buffer_history_get_record_info(app, buffer_id, index);
    if (record.error == RecordError_NoError && record.kind == RecordKind_Group){
        record = buffer_history_get_group_sub_record(app, buffer_id, index, record.group.count - 1);
    }
    return(record);
}

////////////////////////////////

internal Vec2
draw_string(Application_Links *app, Face_ID font_id, String_Const_u8 string, Vec2 p, int_color color){
    return(draw_string_oriented(app, font_id, string, p, color, 0, V2(1.f, 0.f)));
}

internal void
draw_margin(Application_Links *app, Rect_f32 outer, Rect_f32 inner, int_color color){
    draw_rectangle(app, Rf32(outer.x0, outer.y0, outer.x1, inner.y0), color);
    draw_rectangle(app, Rf32(outer.x0, inner.y1, outer.x1, outer.y1), color);
    draw_rectangle(app, Rf32(outer.x0, inner.y0, inner.x0, inner.y1), color);
    draw_rectangle(app, Rf32(inner.x1, inner.y0, outer.x1, inner.y1), color);
}

internal void
draw_character_block(Application_Links *app, Text_Layout_ID layout, i64 pos, int_color color){
    Rect_f32 rect = text_layout_character_on_screen(app, layout, pos);
    draw_rectangle(app, rect, color);
}

internal void
draw_character_block(Application_Links *app, Text_Layout_ID layout, Range_i64 range, int_color color){
    for (i64 i = range.first; i < range.one_past_last; i += 1){
        draw_character_block(app, layout, i, color);
    }
}

internal void
draw_character_wire_frame(Application_Links *app, Text_Layout_ID layout, i64 pos, int_color color){
    Rect_f32 rect = text_layout_character_on_screen(app, layout, pos);
    draw_rectangle_outline(app, rect, color);
}

internal void
draw_character_wire_frame(Application_Links *app, Text_Layout_ID layout, Range_i64 range, int_color color){
    for (i64 i = range.first; i < range.one_past_last; i += 1){
        draw_character_wire_frame(app, layout, i, color);
    }
}

internal void
draw_character_i_bar(Application_Links *app, Text_Layout_ID layout, i64 pos, int_color color){
    Rect_f32 rect = text_layout_character_on_screen(app, layout, pos);
    rect.x1 = rect.x0 + 1.f;
    draw_rectangle(app, rect, color);
}

internal void
draw_line_highlight(Application_Links *app, Text_Layout_ID layout, Range_i64 line_range, int_color color){
    Range_f32 y1 = text_layout_line_on_screen(app, layout, line_range.min);
    Range_f32 y2 = text_layout_line_on_screen(app, layout, line_range.max);
    Range_f32 y = range_union(y1, y2);
    if (range_size(y) > 0.f){
        Rect_f32 region = text_layout_region(app, layout);
        draw_rectangle(app, Rf32(rect_range_x(region), y), color);
    }
}

internal void
draw_line_highlight(Application_Links *app, Text_Layout_ID layout, i64 line, int_color color){
    draw_line_highlight(app, layout, Ii64(line), color);
}

internal void
paint_text_color_pos(Application_Links *app, Text_Layout_ID layout, i64 pos, int_color color){
    paint_text_color(app, layout, Ii64(pos, pos + 1), color);
}

////////////////////////////////

internal Rect_f32_Pair
split_rect(Rect_f32 rect, View_Split_Kind kind, Coordinate coord, Side from_side, f32 t){
    Rect_f32_Pair result = {};
    if (kind == ViewSplitKind_FixedPixels){
        result.e[0] = rect;
        result.e[1] = rect;
        if (coord == Coordinate_X){
            result.e[0].x1 = (from_side == Side_Max) ? (rect.x1 - t) : (rect.x0 + t);
            result.e[1].x0 = result.e[0].x1;
        }
        else{
            Assert(coord == Coordinate_Y);
            result.e[0].y1 = (from_side == Side_Max) ? (rect.y1 - t) : (rect.y0 + t);
            result.e[1].y0 = result.e[0].y1;
        }
    }
    else{
        Assert(kind == ViewSplitKind_Ratio);
        f32 pixel_count;
        if (coord == Coordinate_X){
            pixel_count = t*(rect.x1 - rect.x0);
        }
        else{
            Assert(coord == Coordinate_Y);
            pixel_count = t*(rect.y1 - rect.y0);
        }
        result = split_rect(rect, ViewSplitKind_FixedPixels, coord, from_side, pixel_count);
    }
    return(result);
}

////////////////////////////////

static int_color
get_margin_color(i32 level){
    int_color margin = 0;
    switch (level){
        default:
        case UIActivation_None:
        {
            margin = Stag_List_Item;
        }break;
        case UIActivation_Hover:
        {
            margin = Stag_List_Item_Hover;
        }break;
        case UIActivation_Active:
        {
            margin = Stag_List_Item_Active;
        }break;
    }
    return(margin);
}

////////////////////////////////

UI_QUIT_FUNCTION(ui_quit_clear_render_hook){
    Managed_Scope scope = view_get_managed_scope(app, view);
    View_Render_Hook **hook = scope_attachment(app, scope, view_render_hook, View_Render_Hook*);
    *hook = 0;
}

////////////////////////////////

// TODO(allen): REWRITE THIS EXACTLY HOW YOU WANT IT --- start ---

internal Child_Process_Set_Target_Flags
flags_system_command(Command_Line_Interface_Flag flags){
    Child_Process_Set_Target_Flags set_buffer_flags = 0;
    if (!HasFlag(flags, CLI_OverlapWithConflict)){
        set_buffer_flags |= ChildProcessSet_FailIfBufferAlreadyAttachedToAProcess;
    }
    if (HasFlag(flags, CLI_CursorAtEnd)){
        set_buffer_flags |= ChildProcessSet_CursorAtEnd;
    }
    return(set_buffer_flags);
}

internal b32
set_buffer_system_command(Application_Links *app, Child_Process_ID process, Buffer_ID buffer, Command_Line_Interface_Flag flags){
    b32 result = false;
    Child_Process_Set_Target_Flags set_buffer_flags = flags_system_command(flags);
    if (child_process_set_target_buffer(app, process, buffer, set_buffer_flags)){
        clear_buffer(app, buffer);
        if (HasFlag(flags, CLI_SendEndSignal)){
            buffer_send_end_signal(app, buffer);
        }
        result = true;
    }
    return(result);
}

internal b32
exec_system_command(Application_Links *app, View_ID view, Buffer_Identifier buffer_id,
                    String_Const_u8 path, String_Const_u8 command, Command_Line_Interface_Flag flags){
    b32 result = false;
    Child_Process_ID child_process_id = 0;
    if (create_child_process(app, path, command, &child_process_id)){
        result = true;
        Buffer_ID buffer_attach_id = buffer_identifier_to_id_create_out_buffer(app, buffer_id);
        if (buffer_attach_id != 0){
            if (set_buffer_system_command(app, child_process_id, buffer_attach_id, flags)){
                if (view != 0){
                    view_set_buffer(app, view, buffer_attach_id, 0);
                    view_set_cursor(app, view, seek_pos(0));
                }
            }
        }
    }
    return(result);
}

// TODO(allen): --- end ---

// BOTTOM
