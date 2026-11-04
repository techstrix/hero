-- rc.local
print("Loading window manager configuration...")

local success = set_wallpaper("./assets/wall2.jpg")

if success then
	print("Wallpaper applied successfully from Lua!")
else
	print("Failed to apply wallpaper.")
end
