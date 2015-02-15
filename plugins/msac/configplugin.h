#ifndef _CONFIGPLUGIN_H_
#define _CONFIGPLUGIN_H_
#include <string>
#include <Window.h>
#include <String.h>
#define SPEED115200 'SP0'
#define SPEED57600 'SP1'
#define SPEED38400 'SP2'
#define SPEED19200 'SP3'
#define SPEED9600 'SP4'
#define CANCELPORTWINDOW 'CSW'

class configplugin : public BWindow
{
public:
	configplugin(BRect rect);
	virtual bool	QuitRequested();
	virtual void	MessageReceived(BMessage* message);
private:
	BView* speedview;
	BRadioButton *radioButton_L2S111;
	BRadioButton *radioButton_L2S112;
	BRadioButton *radioButton_L2S113;
	BRadioButton *radioButton_L2S114;
	BRadioButton *radioButton_L2S115;
	BPopUpMenu* menu;
};

#endif
