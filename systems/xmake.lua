-- 获取路径的最后一段作为文件夹名
local function get_folder_name(path)
    return path:match("[^/\\]+$")
end

-- 获取当前脚本所在目录
local script_dir = os.scriptdir()

-- 遍历所有子目录并定义target
for _, dir in ipairs(os.dirs(path.join(script_dir, "*"))) do

    local folder_name = get_folder_name(dir)
    local target_name = get_folder_name(script_dir)
    -- 定义target
    target(target_name.."_"..folder_name)
        set_kind("binary")
        add_files(folder_name .. "/src/**.cpp")
        add_includedirs(folder_name .. "/include")
end


-- # 构建所有目标
-- xmake
-- # 构建特定目标
-- xmake build target_name