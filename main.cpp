// Copyright (c) 2010  Paolo Capriotti <p.capriotti@gmail.com>
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
// 
// 1. Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
// 
// THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
// IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
// OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
// IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
// INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
// NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
// THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include <KApplication>
#include <KAboutData>
#include <KCmdLineArgs>

#include "mainwindow.h"

int main(int argc, char** argv)
{
  KAboutData aboutData(
    "hikki",
    0,
    ki18n("Hikki"),
    "0.1",
    ki18n("Minimalistic Note-taking Tool"),
    KAboutData::License_BSD,
    ki18n("(c) 2010 Paolo Capriotti"));
  
  KCmdLineOptions opt;
  opt.add("+[notebook]", ki18n("Notebook path"));
  KCmdLineArgs::addCmdLineOptions(opt);

  KCmdLineArgs::init(argc, argv, &aboutData);
  KApplication app;

  QString base;
  if (const char* baseEnv = getenv("HIKKI_DIR")) {
    base = baseEnv;
  }
  else {
    base = ".";
  }
  
  KCmdLineArgs* args = KCmdLineArgs::parsedArgs();
  if (args->count() > 0) {
    base = args->arg(0);
  }
  
  MainWindow* main = new MainWindow(base);
  main->show();
  
  return app.exec();
}
