bullet = function()
    __class_name = "bullet"
    __base_node = sprite
    __props = {
        move_speed = 30,
        hit_player = false,
        hit_enemy = false,
    }

    function fixed_update()
        self:translate(self.velocity * self.move_speed)

        local x, y = self:position()
        local w, h = view_size()
        if x < -screen_bound_buffer or x > w + screen_bound_buffer
        or y < -screen_bound_buffer or y > h + screen_bound_buffer then
            self:mark_for_deletion()
        end

        -- todo: do some hit detection
    end

    print("bullet!: ", __class_name)
end

bullet()