"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
const puerts_1 = require("puerts");
const cpp = require("cpp");
puerts_1.argv.getByName("ImGuiBridge").OnGUI.Add(OnGUI);
let ImGui = cpp.UnrealImGui;
function OnGUI() {
    ImGui.ShowDemoWindow(null);
}
//# sourceMappingURL=Main.js.map