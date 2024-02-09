function init()
    local ship_script = load_script("assets/scripts/space/player_ship.lua")
    local ship = create_sprite_node(
        0,
        -100,
        -100
    )
    ship:script(ship_script)
end