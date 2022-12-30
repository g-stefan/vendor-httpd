// Created by Grigore Stefan <g_stefan@yahoo.com>
// Public domain (Unlicense) <http://unlicense.org>
// SPDX-FileCopyrightText: 2022 Grigore Stefan <g_stefan@yahoo.com>
// SPDX-License-Identifier: Unlicense

messageAction("install");

forceRemoveDirRecursively(pathRepository + "/opt/httpd");
exitIf(!Shell.copyDirRecursively("output", pathRepository + "/opt/httpd"));
