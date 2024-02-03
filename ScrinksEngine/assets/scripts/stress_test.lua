function init()
    local sprite_script = load_script("assets/scripts/sprite.lua")
    local root = get_root_node()
    for i = 1, 30000 do
        local sprite = create_sprite_node(
            (math.random() * 3),
            math.random() * 1920,
            math.random() * 1080
        )
        sprite:rename("lua@sprite[" .. i .. "]")
        sprite:set_script(sprite_script)
    end
end