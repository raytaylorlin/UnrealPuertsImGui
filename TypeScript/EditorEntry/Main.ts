import {$ref, $unref, $set, argv} from 'puerts';
import * as UE from 'ue';
import * as cpp from 'cpp'
import { ImGuiMouseButton, ImGuiWindowFlags } from './ImGui/Enum';
import { combineCombo } from './ImGui/Helper';

(argv.getByName("ImGuiBridge") as UE.PuertsXImGuiBridge).OnGUI.Add(OnGUI);
let ImGui = cpp.UnrealImGui;

function OnGUI() {
    ImGui.ShowDemoWindow(null)
}
