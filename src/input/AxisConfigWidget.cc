/*
 *  OpenSCAD (www.openscad.org)
 *  Copyright (C) 2009-2014 Clifford Wolf <clifford@clifford.at> and
 *                          Marius Kintel <marius@kintel.net>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  As a special exception, you have permission to link this program
 *  with the CGAL library and distribute executables, as long as you
 *  follow the requirements of the GNU GPL in regard to all of the
 *  software in the executable aside from CGAL.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include <QWidget>
#include "AxisConfigWidget.h"

#include "settings.h"
#include "QSettingsCached.h"
#include "input/InputDriverManager.h"
#include "SettingsWriter.h"

AxisConfigWidget::AxisConfigWidget(QWidget *parent) : QWidget(parent)
{
	setupUi(this);
	//init();
}

AxisConfigWidget::~AxisConfigWidget()
{
}

void AxisConfigWidget::AxesChanged(int nr, double val) const{
	int value = val *100;

	QString s =  QString::number(val, 'f', 2 );
	std::string number = std::to_string(nr);
	QProgressBar* progressBar = this->findChild<QProgressBar *>(QString::fromStdString("progressBarAxis"+number));
	if(progressBar==0) return;
	progressBar->setValue(value);
	progressBar->setFormat(s);
}

void AxisConfigWidget::init() {
	connect(this->pushButtonAxisTrim, SIGNAL(clicked()), this, SLOT(on_AxisTrim()));
	connect(this->pushButtonAxisTrimReset, SIGNAL(clicked()), this, SLOT(on_AxisTrimReset()));

	initComboBox(this->comboBoxTranslationX, Settings::Settings::inputTranslationX);
	initComboBox(this->comboBoxTranslationY, Settings::Settings::inputTranslationY);
	initComboBox(this->comboBoxTranslationZ, Settings::Settings::inputTranslationZ);
	initComboBox(this->comboBoxTranslationXVPRel, Settings::Settings::inputTranslationXVPRel);
	initComboBox(this->comboBoxTranslationYVPRel, Settings::Settings::inputTranslationYVPRel);
	initComboBox(this->comboBoxTranslationZVPRel, Settings::Settings::inputTranslationZVPRel);
	initComboBox(this->comboBoxRotationX, Settings::Settings::inputRotateX);
	initComboBox(this->comboBoxRotationY, Settings::Settings::inputRotateY);
	initComboBox(this->comboBoxRotationZ, Settings::Settings::inputRotateZ);
	initComboBox(this->comboBoxZoom, Settings::Settings::inputZoom);
	initComboBox(this->comboBoxZoom2, Settings::Settings::inputZoom2);

	updateComboBox(this->comboBoxTranslationX, Settings::Settings::inputTranslationX);
	updateComboBox(this->comboBoxTranslationY, Settings::Settings::inputTranslationY);
	updateComboBox(this->comboBoxTranslationZ, Settings::Settings::inputTranslationZ);
	updateComboBox(this->comboBoxTranslationXVPRel, Settings::Settings::inputTranslationXVPRel);
	updateComboBox(this->comboBoxTranslationYVPRel, Settings::Settings::inputTranslationYVPRel);
	updateComboBox(this->comboBoxTranslationZVPRel, Settings::Settings::inputTranslationZVPRel);
	updateComboBox(this->comboBoxRotationX, Settings::Settings::inputRotateX);
	updateComboBox(this->comboBoxRotationY, Settings::Settings::inputRotateY);
	updateComboBox(this->comboBoxRotationZ, Settings::Settings::inputRotateZ);
	updateComboBox(this->comboBoxZoom, Settings::Settings::inputZoom);
	updateComboBox(this->comboBoxZoom2, Settings::Settings::inputZoom2);


	QString NotEnabledDuringBuild =_("This driver was not enabled during build time and is thus not available.");
#ifdef ENABLE_HIDAPI
	this->checkBoxHIDAPI->setEnabled(true);
#else
	this->checkBoxHIDAPI->setToolTip(NotEnabledDuringBuild);
#endif
#ifdef ENABLE_SPNAV
	this->checkBoxSpaceNav->setEnabled(true);
#else
	this->checkBoxSpaceNav->setToolTip(NotEnabledDuringBuild);
#endif
#ifdef ENABLE_JOYSTICK
	this->checkBoxJoystick->setEnabled(true);
#else
	this->checkBoxJoystick->setToolTip(NotEnabledDuringBuild);
#endif
#ifdef ENABLE_QGAMEPAD
	this->checkBoxQGamepad->setEnabled(true);
#else
	this->checkBoxQGamepad->setToolTip(NotEnabledDuringBuild);
#endif
#ifdef ENABLE_DBUS
	this->checkBoxDBus->setEnabled(true);
#else
	this->checkBoxDBus->setToolTip(NotEnabledDuringBuild);
#endif

	initCheckBox(this->checkBoxHIDAPI,   Settings::Settings::inputEnableDriverHIDAPI);
	initCheckBox(this->checkBoxSpaceNav, Settings::Settings::inputEnableDriverSPNAV);
	initCheckBox(this->checkBoxJoystick, Settings::Settings::inputEnableDriverJOYSTICK);
	initCheckBox(this->checkBoxQGamepad, Settings::Settings::inputEnableDriverQGAMEPAD);
	initCheckBox(this->checkBoxDBus,     Settings::Settings::inputEnableDriverDBUS);

	for (int i = 0; i < InputEventMapper::getMaxAxis(); i++ ){
		std::string s = std::to_string(i);

		QDoubleSpinBox* spin;
		Settings::SettingsEntry* ent;

		spin = this->findChild<QDoubleSpinBox *>(QString::fromStdString("doubleSpinBoxTrim"+s));
		ent = Settings::Settings::inst()->getSettingEntryByName("axisTrim" +s);
		if(spin && ent){
			initDoubleSpinBox(spin,*ent);
		}
		spin = this->findChild<QDoubleSpinBox *>(QString::fromStdString("doubleSpinBoxDeadzone"+s));
		ent = Settings::Settings::inst()->getSettingEntryByName("axisDeadzone" +s);
		if(spin && ent){
			initDoubleSpinBox(spin,*ent);
		}
	}

	for (int i = 0; i < InputEventMapper::getMaxAxis(); i++ ){
		std::string s = std::to_string(i);
		Settings::Settings *setting = Settings::Settings::inst();

		QDoubleSpinBox* spin;
		Settings::SettingsEntry* ent;

		spin= this->findChild<QDoubleSpinBox *>(QString::fromStdString("doubleSpinBoxTrim"+s));
		ent = Settings::Settings::inst()->getSettingEntryByName("axisTrim" +s );
		if(spin && ent){
			spin->setValue((double)setting->get(*ent).toDouble());
		}

		spin= this->findChild<QDoubleSpinBox *>(QString::fromStdString("doubleSpinBoxDeadzone"+s));
		ent = Settings::Settings::inst()->getSettingEntryByName("axisDeadzone" +s );
		if(spin && ent){
			spin->setValue((double)setting->get(*ent).toDouble());
		}
	}
	
	initDoubleSpinBox(this->doubleSpinBoxTranslationGain, Settings::Settings::inputTranslationGain);
	initDoubleSpinBox(this->doubleSpinBoxTranslationVPRelGain, Settings::Settings::inputTranslationVPRelGain);
	initDoubleSpinBox(this->doubleSpinBoxRotateGain, Settings::Settings::inputRotateGain);
	initDoubleSpinBox(this->doubleSpinBoxZoomGain, Settings::Settings::inputZoomGain);
	Settings::Settings *s = Settings::Settings::inst();
	this->doubleSpinBoxRotateGain->setValue((double)s->get(Settings::Settings::inputRotateGain).toDouble());
	this->doubleSpinBoxTranslationGain->setValue((double)s->get(Settings::Settings::inputTranslationGain).toDouble());
	this->doubleSpinBoxTranslationVPRelGain->setValue((double)s->get(Settings::Settings::inputTranslationVPRelGain).toDouble());
	this->doubleSpinBoxZoomGain->setValue((double)s->get(Settings::Settings::inputZoomGain).toDouble());
}

void AxisConfigWidget::on_comboBoxTranslationX_activated(int val)
{
	applyComboBox(comboBoxTranslationX, val, Settings::Settings::inputTranslationX);
        emit inputMappingChanged();
}

void AxisConfigWidget::on_comboBoxTranslationY_activated(int val)
{
	applyComboBox(comboBoxTranslationY, val, Settings::Settings::inputTranslationY);
        emit inputMappingChanged();
}

void AxisConfigWidget::on_comboBoxTranslationZ_activated(int val)
{
	applyComboBox(comboBoxTranslationZ, val, Settings::Settings::inputTranslationZ);
        emit inputMappingChanged();
}

void AxisConfigWidget::on_comboBoxTranslationXVPRel_activated(int val)
{
	applyComboBox(comboBoxTranslationXVPRel, val, Settings::Settings::inputTranslationXVPRel);
        emit inputMappingChanged();
}

void AxisConfigWidget::on_comboBoxTranslationYVPRel_activated(int val)
{
	applyComboBox(comboBoxTranslationYVPRel, val, Settings::Settings::inputTranslationYVPRel);
        emit inputMappingChanged();
}

void AxisConfigWidget::on_comboBoxTranslationZVPRel_activated(int val)
{
	applyComboBox(comboBoxTranslationZVPRel, val, Settings::Settings::inputTranslationZVPRel);
        emit inputMappingChanged();
}
void AxisConfigWidget::on_comboBoxRotationX_activated(int val)
{
	applyComboBox(comboBoxRotationX, val, Settings::Settings::inputRotateX);
        emit inputMappingChanged();
}

void AxisConfigWidget::on_comboBoxRotationY_activated(int val)
{
	applyComboBox(comboBoxRotationY, val, Settings::Settings::inputRotateY);
        emit inputMappingChanged();
}

void AxisConfigWidget::on_comboBoxRotationZ_activated(int val)
{
	applyComboBox(comboBoxRotationZ, val, Settings::Settings::inputRotateZ);
        emit inputMappingChanged();
}

void AxisConfigWidget::on_comboBoxZoom_activated(int val)
{
	applyComboBox(comboBoxZoom, val, Settings::Settings::inputZoom);
        emit inputMappingChanged();
}

void AxisConfigWidget::on_comboBoxZoom2_activated(int val)
{
	applyComboBox(comboBoxZoom2, val, Settings::Settings::inputZoom2);
        emit inputMappingChanged();
}

void AxisConfigWidget::on_doubleSpinBoxTrim0_valueChanged(double val)
{
	Settings::Settings::inst()->set(Settings::Settings::axisTrim0, Value(val));
	emit inputCalibrationChanged();
	writeSettings();
}

void AxisConfigWidget::on_doubleSpinBoxTrim1_valueChanged(double val)
{
	Settings::Settings::inst()->set(Settings::Settings::axisTrim1, Value(val));
	emit inputCalibrationChanged();
	writeSettings();
}

void AxisConfigWidget::on_doubleSpinBoxTrim2_valueChanged(double val)
{
	Settings::Settings::inst()->set(Settings::Settings::axisTrim2, Value(val));
	emit inputCalibrationChanged();
	writeSettings();
}

void AxisConfigWidget::on_doubleSpinBoxTrim3_valueChanged(double val)
{
	Settings::Settings::inst()->set(Settings::Settings::axisTrim3, Value(val));
	emit inputCalibrationChanged();
	writeSettings();
}

void AxisConfigWidget::on_doubleSpinBoxTrim4_valueChanged(double val)
{
	Settings::Settings::inst()->set(Settings::Settings::axisTrim4, Value(val));
	emit inputCalibrationChanged();
	writeSettings();
}

void AxisConfigWidget::on_doubleSpinBoxTrim5_valueChanged(double val)
{
	Settings::Settings::inst()->set(Settings::Settings::axisTrim5, Value(val));
	emit inputCalibrationChanged();
	writeSettings();
}

void AxisConfigWidget::on_doubleSpinBoxTrim6_valueChanged(double val)
{
	Settings::Settings::inst()->set(Settings::Settings::axisTrim6, Value(val));
	emit inputCalibrationChanged();
	writeSettings();
}

void AxisConfigWidget::on_doubleSpinBoxTrim7_valueChanged(double val)
{
	Settings::Settings::inst()->set(Settings::Settings::axisTrim7, Value(val));
	emit inputCalibrationChanged();
}

void AxisConfigWidget::on_doubleSpinBoxTrim8_valueChanged(double val)
{
	Settings::Settings::inst()->set(Settings::Settings::axisTrim8, Value(val));
	emit inputCalibrationChanged();
	writeSettings();
}

void AxisConfigWidget::on_doubleSpinBoxDeadzone0_valueChanged(double val)
{
	Settings::Settings::inst()->set(Settings::Settings::axisDeadzone0, Value(val));
	emit inputCalibrationChanged();
	writeSettings();
}

void AxisConfigWidget::on_doubleSpinBoxDeadzone1_valueChanged(double val)
{
	Settings::Settings::inst()->set(Settings::Settings::axisDeadzone1, Value(val));
	emit inputCalibrationChanged();
	writeSettings();
}

void AxisConfigWidget::on_doubleSpinBoxDeadzone2_valueChanged(double val)
{
	Settings::Settings::inst()->set(Settings::Settings::axisDeadzone2, Value(val));
	emit inputCalibrationChanged();
	writeSettings();
}

void AxisConfigWidget::on_doubleSpinBoxDeadzone3_valueChanged(double val)
{
	Settings::Settings::inst()->set(Settings::Settings::axisDeadzone3, Value(val));
	emit inputCalibrationChanged();
	writeSettings();
}

void AxisConfigWidget::on_doubleSpinBoxDeadzone4_valueChanged(double val)
{
	Settings::Settings::inst()->set(Settings::Settings::axisDeadzone4, Value(val));
	emit inputCalibrationChanged();
	writeSettings();
}

void AxisConfigWidget::on_doubleSpinBoxDeadzone5_valueChanged(double val)
{
	Settings::Settings::inst()->set(Settings::Settings::axisDeadzone5, Value(val));
	emit inputCalibrationChanged();
	writeSettings();
}

void AxisConfigWidget::on_doubleSpinBoxDeadzone6_valueChanged(double val)
{
	Settings::Settings::inst()->set(Settings::Settings::axisDeadzone6, Value(val));
	emit inputCalibrationChanged();
	writeSettings();
}

void AxisConfigWidget::on_doubleSpinBoxDeadzone7_valueChanged(double val)
{
	Settings::Settings::inst()->set(Settings::Settings::axisDeadzone7, Value(val));
	emit inputCalibrationChanged();
	writeSettings();
}

void AxisConfigWidget::on_doubleSpinBoxDeadzone8_valueChanged(double val)
{
	Settings::Settings::inst()->set(Settings::Settings::axisDeadzone8, Value(val));
	emit inputCalibrationChanged();
	writeSettings();
}

void AxisConfigWidget::on_doubleSpinBoxRotateGain_valueChanged(double val)
{
	Settings::Settings::inst()->set(Settings::Settings::inputRotateGain, Value(val));
	emit inputGainChanged();
	writeSettings();
}

void AxisConfigWidget::on_doubleSpinBoxTranslationGain_valueChanged(double val)
{
	Settings::Settings::inst()->set(Settings::Settings::inputTranslationGain, Value(val));
	emit inputGainChanged();
	writeSettings();
}

void AxisConfigWidget::on_doubleSpinBoxTranslationVPRelGain_valueChanged(double val)
{
	Settings::Settings::inst()->set(Settings::Settings::inputTranslationVPRelGain, Value(val));
	emit inputGainChanged();
	writeSettings();
}

void AxisConfigWidget::on_doubleSpinBoxZoomGain_valueChanged(double val)
{
	Settings::Settings::inst()->set(Settings::Settings::inputZoomGain, Value(val));
	emit inputGainChanged();
	writeSettings();
}

void AxisConfigWidget::on_AxisTrim()
{
	InputEventMapper::instance()->onAxisAutoTrim();

	for (int i = 0; i < InputEventMapper::getMaxAxis(); i++ ){
		std::string s = std::to_string(i);

		QDoubleSpinBox* spin;
		Settings::SettingsEntry* ent;

		spin = this->findChild<QDoubleSpinBox *>(QString::fromStdString("doubleSpinBoxTrim"+s));
		ent = Settings::Settings::inst()->getSettingEntryByName("axisTrim" +s);

		if(spin && ent){
			spin->setValue((double)Settings::Settings::inst()->get(*ent).toDouble());
		}
	}
	emit inputCalibrationChanged();
	writeSettings();
}

void AxisConfigWidget::on_AxisTrimReset()
{
	InputEventMapper::instance()->onAxisTrimReset();
	for (int i = 0; i < InputEventMapper::getMaxAxis(); i++ ){
		std::string s = std::to_string(i);

		QDoubleSpinBox* spin;
		Settings::SettingsEntry* ent;

		ent = Settings::Settings::inst()->getSettingEntryByName("axisTrim" +s);
		if(ent){
			Settings::Settings::inst()->set(*ent, 0.00);
		}

		spin = this->findChild<QDoubleSpinBox *>(QString::fromStdString("doubleSpinBoxTrim"+s));
		if(spin){
			spin->setValue(0.00);
		}
	}
	emit inputCalibrationChanged();
	writeSettings();
}

void AxisConfigWidget::on_checkBoxHIDAPI_toggled(bool val)
{
	Settings::Settings::inst()->set(Settings::Settings::inputEnableDriverHIDAPI, Value(val));
	writeSettings();
}

void AxisConfigWidget::on_checkBoxSpaceNav_toggled(bool val)
{
	Settings::Settings::inst()->set(Settings::Settings::inputEnableDriverSPNAV, Value(val));
	writeSettings();
}

void AxisConfigWidget::on_checkBoxJoystick_toggled(bool val)
{
	Settings::Settings::inst()->set(Settings::Settings::inputEnableDriverJOYSTICK, Value(val));
	writeSettings();
}

void AxisConfigWidget::on_checkBoxQGamepad_toggled(bool val)
{
	Settings::Settings::inst()->set(Settings::Settings::inputEnableDriverQGAMEPAD, Value(val));
	writeSettings();
}

void AxisConfigWidget::on_checkBoxDBus_toggled(bool val)
{
	Settings::Settings::inst()->set(Settings::Settings::inputEnableDriverDBUS, Value(val));
	writeSettings();
}

void AxisConfigWidget::applyComboBox(QComboBox *comboBox, int val, Settings::SettingsEntry& entry)
{
	QString s = comboBox->itemData(val).toString();
	Settings::Settings::inst()->set(entry, Value(s.toStdString()));
	writeSettings();
}

void AxisConfigWidget::initSpinBox(QSpinBox *spinBox, const Settings::SettingsEntry& entry)
{
	RangeType range = entry.range().toRange();
	spinBox->setMinimum(range.begin_value());
	spinBox->setMaximum(range.end_value());
}

void AxisConfigWidget::initDoubleSpinBox(QDoubleSpinBox *spinBox, const Settings::SettingsEntry& entry)
{
	RangeType range = entry.range().toRange();
	spinBox->setMinimum(range.begin_value());
	spinBox->setMaximum(range.end_value());
}

void AxisConfigWidget::initCheckBox(QCheckBox *checkBox, const Settings::SettingsEntry& entry)
{
	Settings::Settings *s = Settings::Settings::inst();
	const Value &value = s->get(entry);
	bool state = value.toBool();

	checkBox->setChecked(state);
}

void AxisConfigWidget::updateComboBox(QComboBox *comboBox, const Settings::SettingsEntry& entry)
{
	Settings::Settings *s = Settings::Settings::inst();

	const Value &value = s->get(entry);
	QString text = QString::fromStdString(value.toString());
	int idx = comboBox->findData(text);
	if (idx >= 0) {
		comboBox->setCurrentIndex(idx);
	} else {
		const Value &defaultValue = entry.defaultValue();
		QString defaultText = QString::fromStdString(defaultValue.toString());
		int defIdx = comboBox->findData(defaultText);
		if (defIdx >= 0) {
			comboBox->setCurrentIndex(defIdx);
		} else {
			comboBox->setCurrentIndex(0);
		}
	}
}

void AxisConfigWidget::writeSettings()
{
	SettingsWriter settingsWriter;
	Settings::Settings::inst()->visit(settingsWriter);
}

void AxisConfigWidget::initComboBox(QComboBox *comboBox, const Settings::SettingsEntry& entry)
{
	comboBox->clear();
	// Range is a vector of 2D vectors: [[name, value], ...]
	for(const auto &v : entry.range().toVector()) {
		QString val = QString::fromStdString(v[0]->toString());
		QString qtext = QString::fromStdString(gettext(v[1]->toString().c_str()));
		comboBox->addItem(qtext, val);
	}
}
