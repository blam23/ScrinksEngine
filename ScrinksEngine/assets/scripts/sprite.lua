grab_radius = 100
wander_range = 50
dst_cooldown = (math.random() + 2)
dst_current = math.random() * dst_cooldown

last_x = 0
last_y = 0
start_x = 0
start_y = 0
dst_x = 0
dst_y = 0

function distance_sqrd(ax, ay, bx, by)
    local dx = ax - bx
    local dy = ay - by
    return dx * dx + dy * dy
end

function set_pos2(x, y)
    set_pos(x, y)
    last_x = x
    last_y = y
end

function random_pos(xrange, yrange)
    local x = math.random() * xrange
    local y = math.random() * yrange
    set_pos2(x, y)
    start_x = x
    start_y = y
end

function random_dst(xrange, yrange)
    dst_x = start_x + (math.random()-0.5) * xrange
    dst_y = start_y + (math.random()-0.5) * yrange
end

function script_added()
    random_pos(1920, 1080)
    dst_x, dst_y = start_x, start_y
end

function fixed_update()
    -- todo: get delta
    dst_current = dst_current - 0.04
    if dst_current <= 0 then
        dst_current = dst_cooldown
        random_dst(wander_range,wander_range)
    end

    local mouse_x, mouse_y = get_mouse_position()
    mouse_y = mouse_y - 30
    local dist = distance_sqrd(mouse_x, mouse_y, last_x, last_y)

    local target_x, target_y = dst_x, dst_y
    local move_speed = .1
    if dist < grab_radius * grab_radius then
        dist = math.sqrt(dist)
        target_x, target_y = ((last_x-mouse_x) / dist) * 50, ((last_y-mouse_y) / dist) * 50
        target_x = target_x + last_x
        target_y = target_y + last_y
        move_speed = .3
    end

    local nx = last_x + ((target_x - last_x) * move_speed)
    local ny = last_y + ((target_y - last_y) * move_speed)
    set_pos2(nx, ny)
end
