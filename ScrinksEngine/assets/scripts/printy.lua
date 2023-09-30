i = 0
function fixed_update()
    i = i - 0.1
    y = math.sin(i)
    set_pos(0, (y/2) + 0.5, 0)
end