// For conditions of distribution and use, see copyright notice in license.txt

/**
 *  @file   ScriptDialogWidget.cpp
 *  @brief  Dialog Widget started by llDialog script command at server.
 */

#include "StableHeaders.h"
#include "Communications/ScriptDialogWidget.h"
#include <QFile>
#include <QUiLoader>
#include <QPushButton>
#include <QVBoxLayout>
#include <QPlainTextEdit>
#include <QLabel>
#include <QTableWidget>
#include <QGridLayout>

namespace RexLogic
{
	SelectionButton::SelectionButton(QWidget *parent, const char *name , QString id ): QPushButton(name, parent),  id_(id)
	{
		connect(this, SIGNAL(clicked()), this, SLOT(OnClicked()) );
	}



	void SelectionButton::OnClicked()
	{
		emit Clicked(id_);
	}

	ScriptDialogWidget::ScriptDialogWidget(ScriptDialogRequest &request, Foundation::Framework *framework, QWidget *parent) : request_(request)
	{
		InitWindow(request_);
	}

	ScriptDialogWidget::~ScriptDialogWidget()
	{

	}

	void ScriptDialogWidget::InitWindow(ScriptDialogRequest &request)
	{
		QVBoxLayout* layout = new QVBoxLayout(this);
		layout->setSpacing(0);
		layout->setMargin(0);
		setLayout(layout);

		QUiLoader loader;
		QFile file("./data/ui/scriptdialog.ui");
		if (!file.exists())
		{
			// TODO: LogError ("Cannot find ScriptDialog.ui file.");
			return;
		}

		// Get pointers to widgets.
		widget_ = loader.load(&file, 0);
		file.close();

		layout->addWidget(widget_);

		QPlainTextEdit* message_text = widget_->findChild<QPlainTextEdit*>("messageText");
		if (message_text)
			message_text->appendPlainText( QString::fromUtf8(request_.GetMessageText().c_str(), request_.GetMessageText().size()) );

		QLabel* from_text = widget_->findChild<QLabel*>("fromText");
		if (from_text)
			from_text->setText( QString( request_.GetObjectName().c_str()) );

		QLabel* owner_text = widget_->findChild<QLabel*>("ownerText");
		if (owner_text)
			owner_text->setText( QString(request_.GetObjectsOwnerName().c_str()) );

		ButtonLabels labels = request.GetButtonLabels();

		QGridLayout* button_container = widget_->findChild<QGridLayout*>("dynamicButtonsContainer");
		if (!button_container)
		{
			// todo: LogError: "Cannot show buttons on ScriptDialogWidget, button container pointer not valid."
			return;
		}

		//button_container->setColumnCount(3);
		//button_container->setRowCount((labels.size()-1) / 3 + 1);
		
		int index = 0;
		for (ButtonLabels::iterator i = labels.begin(); i != labels.end(); ++i)
		{
			QString label = QString((*i).c_str());
			SelectionButton* button = new SelectionButton(0, label.toStdString().c_str(), label);
			
			int column = index % 3;
			int row = index / 3;

			button_container->addWidget(button, row, column);
			connect(button, SIGNAL( Clicked(QString)), this, SLOT( OnButtonPressed(QString)));
			index++;
		}

		QPushButton* default_button = widget_->findChild<QPushButton*>("defaultButton");
		if (default_button)
		{
			if (labels.size() == 0)
			{
				SelectionButton* button = new SelectionButton(0, "Ok", "Ok");
				button_container->addWidget(button, 0, 0);
				connect(button, SIGNAL( Clicked(QString)), this, SLOT( OnButtonPressed(QString)));
			}
			else
			{
				default_button->setText("Ignore");
				connect(default_button, SIGNAL( clicked()), this, SLOT( OnIgnorePressed()) );
			}
		}
	}

	void ScriptDialogWidget::hideEvent(QHideEvent *hide_event)
	{
		QWidget::hideEvent(hide_event);
		OnIgnorePressed();
	}

	void ScriptDialogWidget::OnButtonPressed(QString id)
	{
		close();
		emit OnClosed(request_.GetChannel(), id);
	}

	void ScriptDialogWidget::OnIgnorePressed()
	{
		close();
		emit OnClosed(request_.GetChannel(), "");
	}

} // end of namespace: RexLogic
