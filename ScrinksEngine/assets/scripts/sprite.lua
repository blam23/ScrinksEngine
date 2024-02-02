i = 0
update = (((math.random()-0.5) * 4) + 4) / 10
radius = (math.random() * 10) + 1
last_x = 0
last_y = 0
grab_radius = 100

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
    x = math.random() * xrange
    y = math.random() * yrange
    set_pos2(x, y)
end

function script_added()
    random_pos(1900, 1000)
end

function fixed_update()
    local mouse_x, mouse_y = get_mouse_position()
    mouse_y = mouse_y - 30
    local sqr_dist = distance_sqrd(mouse_x, mouse_y, last_x, last_y)

    if sqr_dist < grab_radius * grab_radius then
        local nx = last_x + ((mouse_x - last_x) * .1)
        local ny = last_y + ((mouse_y - last_y) * .1)
        set_pos2(nx, ny)
    else
        i = i + update
        set_pos2(x + math.sin(i) * radius, y + math.cos(i) * radius)
    end
end
