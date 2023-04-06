// Created by Grigore Stefan <g_stefan@yahoo.com>
// Public domain (Unlicense) <http://unlicense.org>
// SPDX-FileCopyrightText: 2022-2023 Grigore Stefan <g_stefan@yahoo.com>
// SPDX-License-Identifier: Unlicense

Fabricare.include("vendor");

messageAction("make");

if (Shell.fileExists("temp/build.done.flag")) {
	return;
};

if (!Shell.directoryExists("source")) {
	exitIf(Shell.system("7z x -aoa archive/" + Project.vendor + ".7z"));
	Shell.rename(Project.vendor, "source");
};

Shell.mkdirRecursivelyIfNotExists("output");
Shell.mkdirRecursivelyIfNotExists("output/bin");
Shell.mkdirRecursivelyIfNotExists("output/include");
Shell.mkdirRecursivelyIfNotExists("output/lib");
Shell.mkdirRecursivelyIfNotExists("temp");

Shell.mkdirRecursivelyIfNotExists("temp/cmake");

// required
if (Shell.directoryExists("../vendor-pcre/output")) {
	exitIf(!Shell.copyDirRecursively("../vendor-pcre/output", "output"));
} else {
	Shell.mkdirRecursivelyIfNotExists("vendor");
	// ---
	var vendor = "pcre-8.45-" + Platform.name + "-dev.7z";
	if (Shell.fileExists(pathRelease + "/" + vendor)) {
		Shell.copyFile(pathRelease + "/" + vendor, "vendor/" + vendor);
	} else if (Shell.fileExists("../vendor-pcre/release/" + vendor)) {
		Shell.copyFile("../vendor-pcre/release/" + vendor, "vendor/" + vendor);
	} else {
		var webLink = "https://github.com/g-stefan/vendor-pcre/releases/download/v8.45/" + vendor;
		exitIf(Shell.system("curl --insecure --location " + webLink + " --output vendor/" + vendor));
	};
	exitIf(Shell.system("7z x -aoa -ooutput/ vendor/" + vendor));
};
// ---
if (Shell.directoryExists("../vendor-apr-util/output")) {
	exitIf(!Shell.copyDirRecursively("../vendor-apr-util/output", "output"));
} else {
	Shell.mkdirRecursivelyIfNotExists("vendor");
	var vendor = "apr-util-1.6.3-" + Platform.name + "-dev.7z";
	if (Shell.fileExists(pathRelease + "/" + vendor)) {
		Shell.copyFile(pathRelease + "/" + vendor, "vendor/" + vendor);
	} else if (Shell.fileExists("../vendor-apr-util/release/" + vendor)) {
		Shell.copyFile("../vendor-apr-util/release/" + vendor, "vendor/" + vendor);
	} else {
		var webLink = "https://github.com/g-stefan/vendor-apr-util/releases/download/v1.6.3/" + vendor;
		exitIf(Shell.system("curl --insecure --location " + webLink + " --output vendor/" + vendor));
	};
	exitIf(Shell.system("7z x -aoa -ooutput/ vendor/" + vendor));
};
// ---

if (!Shell.fileExists("temp/build.config.flag")) {
	Shell.copyFile("fabricare/CMakeLists.txt", "source/CMakeLists.txt");

	Shell.setenv("CC", "cl.exe");
	Shell.setenv("CXX", "cl.exe");

	cmdConfig = "cmake";
	cmdConfig += " ../../source";
	cmdConfig += " -G \"Ninja\"";
	cmdConfig += " -DCMAKE_BUILD_TYPE=Release";
	cmdConfig += " -DCMAKE_INSTALL_PREFIX=" + Shell.realPath(Shell.getcwd()) + "\\output";
	cmdConfig += " -DCMAKE_PREFIX_PATH=" + pathRepository;

	runInPath("temp/cmake", function() {
		exitIf(Shell.system(cmdConfig));
	});

	Shell.filePutContents("temp/build.config.flag", "done");
};

runInPath("temp/cmake", function() {
	exitIf(Shell.system("ninja"));
	exitIf(Shell.system("ninja install"));
	exitIf(Shell.system("ninja clean"));
});

var compileProject = {
	"project" : "rotatelogsw",
	"includePath" : [ "output/include", "source" ],
	"cppSource" : [ "fabricare/source/rotatelogsw.cpp" ],
	"library" : [
		"libapr-1"
	]
};

Project.dependency = [
	"xyo-system"
];

compileExeStatic(compileProject);

Shell.filePutContents("temp/build.done.flag", "done");
