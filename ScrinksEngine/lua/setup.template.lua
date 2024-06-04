--Added to all classes.

function new(...)
    local ret = %BASE_NODE%.new(...)
    for k,v in pairs(__props) do
        ret:property(k, v)
    end
    ret:script(load_script("%SCRIPT_PATH%"))
    return ret
end