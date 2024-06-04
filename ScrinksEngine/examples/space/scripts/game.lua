function init()
    print("boom goes the dynamite")

    -- layer = sprite_layer.new({
    --     sheet = "textures/space.png",
    --     tiles_per_row = 16,
    --     tile_size = vec2.new(64,64),
    --     instance_count = 100000
    -- })

    local ship = player.new({
        position = vec2.new(100, 100),
        layer = layer
    })
end
