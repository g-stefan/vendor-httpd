//
// vendor-httpd.make
//
// Copyright (c) 2020 Grigore Stefan <g_stefan@yahoo.com>
// Created by Grigore Stefan <g_stefan@yahoo.com>
//
// MIT License (MIT) <http://opensource.org/licenses/MIT>
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "xyo.hpp"

namespace Main {

	using namespace XYO;

	class Application :
		public virtual IMain {
			XYO_DISALLOW_COPY_ASSIGN_MOVE(Application);
		public:
			String includePath;
			String sourcePath;
			String buildPath;
			String utilPath;
			String binPath;
			String libPath;
			String testPath;
			String workspacePath;

			inline Application() {};

			void showUsage();
			void showLicense();

			int main(int cmdN, char *cmdS[]);
	};

	void Application::showUsage() {
		printf("Scope: build httpd.\n");
		printf("%s\n\n", XYO::Copyright::fullCopyright());

		printf("%s",
			"options:\n"
			"    --license           show license\n"
		);
		printf("\n");
	};

	void Application::showLicense() {
		printf("%s", XYO::License::content());
	};

	int Application::main(int cmdN, char *cmdS[]) {
		int i;
		String opt;
		size_t optIndex;
		String optValue;

		String softwareName = "httpd";
		String softwareVersion = "2.4.39";

		String applicationPath = Shell::getExecutablePath();
		String pathRepository = Shell::getEnv("XYO_PATH_REPOSITORY");
		if(pathRepository.length() == 0) {
			pathRepository = "../repository";
		};

		bool isRelease = true;
		bool doInstall = false;
		bool doRelease = false;
		bool cmdArchive = false;
		bool cmdExtract = false;

		for (i = 1; i < cmdN; ++i) {
			if (StringCore::beginWith(cmdS[i], "--")) {
				opt = &cmdS[i][2];
				optValue = "";
				if(String::indexOf(opt, "=", 0, optIndex)) {
					optValue = String::substring(opt, optIndex + 1);
					opt = String::substring(opt, 0, optIndex);
				};
				if (opt == "license") {
					showLicense();
					return 0;
				};
				continue;
			};
			if (strcmp(cmdS[i], "debug") == 0) {
				isRelease = false;
			};
			if (strcmp(cmdS[i], "install") == 0) {
				doInstall = true;
			};
			if (strcmp(cmdS[i], "do-release") == 0) {
				doRelease = true;
			};
			if (strcmp(cmdS[i], "archive") == 0) {
				cmdArchive = true;
			};
			if (strcmp(cmdS[i], "extract") == 0) {
				cmdExtract = true;
			};
			if (strcmp(cmdS[i], "test") == 0) {
				return 0;
			};
			if (strcmp(cmdS[i], "version") == 0) {
				return 0;
			};
		};

		if(!Shell::realPath(applicationPath + "/..", workspacePath)) {
			printf("ERROR: unable to get workspace real path\n");
			return 1;
		};

		// Archive

		Shell::chdir(workspacePath);

		String workspaceSource = softwareName + "-" + softwareVersion;

		if(cmdArchive) {
			Shell::chdir(workspacePath);

			// Make archive
			if(Shell::directoryExists(workspaceSource)) {
				String archive = "archive/";
				archive << workspaceSource << ".7z";
				if(Shell::fileExists(archive)) {
					if(!Shell::remove(archive)) {
						printf("Error: Unable to remove source archive\r\n");
						return 1;
					};
				};
				if(Shell::system(String("7zr a -mx9 -mmt4 -r- -sse -w. -y -t7z ") + archive + " " + workspaceSource)) {
					printf("Error: Unable to compress source\r\n");
					return 1;
				};
				return 0;
			};
			printf("Error: Source not found\r\n");
			return 1;
		};

		// Extract archive
		if(!Shell::directoryExists(workspaceSource)) {
			if(Shell::system(String("7zr x ") + workspacePath + "/archive/" + workspaceSource + ".7z")) {
				printf("Error: Unable to extract source archive: %s\r\n", workspaceSource.value());
				return 1;
			};
		};

		if(cmdExtract) {
			return 0;
		};

		// Build

		String pathRepositoryX;

		if(!Shell::realPath(pathRepository, pathRepositoryX)) {
			printf("ERROR: unable to get workspace real path to repository\n");
			return 1;
		};

		includePath = workspacePath;
		sourcePath = workspacePath;
		buildPath = workspacePath + "/build";
		binPath = workspacePath + "/bin";
		libPath = workspacePath + "/lib";

		if(!Shell::fileExists(workspacePath + "/build/done.flag")) {

			if(!Shell::copyDirRecursively("../vendor-pcre/build/pcre/lib", buildPath + "/httpd/lib")) {
				printf("pcre not found\n");
				return 1;
			};

			if(!Shell::copyDirRecursively("../vendor-apr/build/apr/include", buildPath + "/httpd/include")) {
				printf("apr not found\n");
				return 1;
			};

			if(!Shell::copyDirRecursively("../vendor-apr/build/apr/lib", buildPath + "/httpd/lib")) {
				printf("apr not found\n");
				return 1;
			};

			if(!Shell::copyDirRecursively("../vendor-apr-util/build/apr-util/include", buildPath + "/httpd/include")) {
				printf("apr-util not found\n");
				return 1;
			};

			if(!Shell::copyDirRecursively("../vendor-apr-util/build/apr-util/lib", buildPath + "/httpd/lib")) {
				printf("apr-util not found\n");
				return 1;
			};


			Shell::mkdirRecursivelyIfNotExists(buildPath + "\\cmake");
			Shell::chdir(buildPath + "\\cmake");

			Shell::setenv("INCLUDE", pathRepositoryX + "\\include;" + Shell::getEnv("INCLUDE"));
			Shell::setenv("LIB", pathRepositoryX + "\\lib;" + Shell::getEnv("LIB"));

			String config;
			config << "cmake";
			config << " ../../" << workspaceSource;
			config << " -G \"NMake Makefiles\"";
			config << " -DCMAKE_BUILD_TYPE=Release";
			config << " -DCMAKE_INSTALL_PREFIX=\"" << buildPath << "\\" << softwareName << "\"";

			Shell::system(config);
			Shell::system("nmake");
			Shell::system("nmake install");
			Shell::system("nmake clean");

			Shell::filePutContents(workspacePath + "/build/done.flag", "done");
			Shell::chdir(workspacePath);

		};

		if(doRelease) {
			String pathRelease = Compiler::getPathReleaseVersion(softwareName, softwareVersion, isRelease);
			// Binary

			if(!Shell::copyDirRecursively(buildPath + "/httpd/bin", pathRelease + "/bin")) {
				return 1;
			};

			if(!Shell::copyDirRecursively(buildPath + "/httpd/cgi-bin", pathRelease + "/cgi-bin")) {
				return 1;
			};
			if(!Shell::copyDirRecursively(buildPath + "/httpd/conf", pathRelease + "/conf")) {
				return 1;
			};
			if(!Shell::copyDirRecursively(buildPath + "/httpd/error", pathRelease + "/error")) {
				return 1;
			};
			if(!Shell::copyDirRecursively(buildPath + "/httpd/htdocs", pathRelease + "/htdocs")) {
				return 1;
			};
			if(!Shell::copyDirRecursively(buildPath + "/httpd/icons", pathRelease + "/icons")) {
				return 1;
			};
			if(!Shell::copyDirRecursively(buildPath + "/httpd/logs", pathRelease + "/logs")) {
				return 1;
			};
			if(!Shell::copyDirRecursively(buildPath + "/httpd/manual", pathRelease + "/manual")) {
				return 1;
			};
			if(!Shell::copyDirRecursively(buildPath + "/httpd/modules", pathRelease + "/modules")) {
				return 1;
			};

			// Dev
			doInstall = true;
			pathRepository = pathRelease + "-dev";
		};

		if(doInstall) {

			if(!Shell::copyDirRecursively(buildPath + "/httpd", pathRepository)) {
				return 1;
			};

		};

		return 0;
	};

};

XYO_APPLICATION_MAIN_STD(Main::Application);
