#include "application.hh"

#include <QFile>
#include <QTextStream>
#include <QMainWindow>
#include <QMessageBox>

#include "logger.hh"
#include "settings.hh"

#include <stdio.h>
#include <QSplashScreen>



int main(int argc, char *argv[]) {
	QTextStream out(stderr);
	Logger::get().addHandler(new StreamLogHandler(out));

	Application app(argc, argv);

#if 0
	QPixmap pixmap(":/icons/splash.png");
	QSplashScreen splash(pixmap);
	splash.show();
	app.processEvents();
#endif

	QMainWindow *mainWindow = app.mainWindow();
	mainWindow->show();

#if 0
	splash.finish(mainWindow);
#endif

	Settings settings;
	if (settings.showDisclaimer()) {
		app.showAbout();
		settings.setShowDisclaimer(false);
	}

	app.exec();

	return (0);
}
