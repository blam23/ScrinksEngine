i = 0
update = (((math.random()-0.5) * 4) + 4) / 10
offset = (math.random()-0.5)*2
offset1 = (math.random()-0.5)*2
offset2 = (math.random()-0.5)*2
interval = 30

function random_pos(range)
    x = (math.random() - 0.5) * range * 2
    y = (math.random() - 0.5) * range * 2
    set_pos(x, 0, y)
end

function script_added()
    random_pos(6)

    set_scale(0.5,0.5,0.5)
    set_rotation(0.0, 0.0, 0.0)
end

function fixed_update()
    i = i + update
    --rotate(0.1, offset, offset1, offset2)
    scale = (math.sin(i + interval) + 5) / 5.0
    set_scale(scale, scale, scale)
    set_pos(x, math.sin(i) + 2, y)
end
