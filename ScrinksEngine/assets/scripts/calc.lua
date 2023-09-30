x = 1
function fixed_update()
    -- do some stuff each frame
    x = x + math.sin(x) - math.cos(x) * math.tan(x)
end