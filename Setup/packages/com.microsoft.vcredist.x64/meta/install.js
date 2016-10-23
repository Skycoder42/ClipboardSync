function Component(){}

Component.prototype.createOperations = function()
{
    try {
        component.createOperations();
		//install visual studio redistributables (f windows) and delete the file right after
		if (installer.value("os") === "win")
			component.addElevatedOperation("Execute", "@TargetDir@/vcredist_x64.exe", "/quiet", "/norestart");
		component.addElevatedOperation("Delete", "@TargetDir@/vcredist_x64.exe");
    } catch (e) {
        print(e);
    }
}
