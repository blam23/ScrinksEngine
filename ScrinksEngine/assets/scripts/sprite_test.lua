grab_radius = 100
wander_range = 100
dst_cooldown = (math.random() + 2)
dst_current = math.random() * dst_cooldown

last_x = 0
last_y = 0
start_x = 0
start_y = 0
dst_x = 0
dst_y = 0

speed = math.random() * 10

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
    random_pos(1900, 1000)
    dst_x, dst_y = start_x, start_y
end

function fixed_update()
    -- todo: get delta
    dst_current = dst_current - 0.04
    if dst_current <= 0 then
        dst_current = dst_cooldown
        random_dst(wander_range,wander_range)
    end

    if is_key_down(546) then
        dst_x = dst_x - 1 * speed
    end

    if is_key_down(549) then
        dst_x = dst_x + 1 * speed
    end

    if is_key_down(568) then
        dst_y = dst_y - 1 * speed
    end

    if is_key_down(564) then
        dst_y = dst_y + 1 * speed
    end

    local target_x, target_y = dst_x, dst_y
    local nx = last_x + ((target_x - last_x) * .1)
    local ny = last_y + ((target_y - last_y) * .1)
    set_pos2(nx, ny)
end
