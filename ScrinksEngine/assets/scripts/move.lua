i = 0
update = (((math.random()-0.5) * 4) + 1) / 10
interval = 2

function random_pos(range)
    x = (math.random() - 0.5) * range * 2
    y = (math.random() - 0.5) * range * 2
    set_pos(x, 0, y)
end

function script_added()
    random_pos(8)
    set_scale(1.0, 1.0, 1.0)
end

function fixed_update()
    i = i + update
    set_pos(x + math.sin(i) * interval, 0, y + math.cos(i) * interval)

    rotate(update, 0, 1, 0)
end
