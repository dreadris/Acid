﻿#include "UiInputText.hpp"

#include "Devices/Keyboard.hpp"
#include "Maths/Visual/DriverSlide.hpp"
#include "Uis/Uis.hpp"
#include "UiInputButton.hpp"

namespace acid
{
UiInputText::UiInputText(UiObject *parent, const std::string &title, const std::string &value, const int32_t &maxLength, const UiBound &rectangle) :
	UiObject(parent, rectangle),
	m_background(this, UiBound::Maximum, Image2d::Create("Guis/Button.png"), UiInputButton::PrimaryColour),
	m_textTitle(this, UiBound(Vector2f(1.0f - (2.5f * UiInputButton::Padding.m_x), 0.5f), UiReference::CentreRight, UiAspect::Position | UiAspect::Size),
		UiInputButton::FontSize, title, FontType::Create("Fonts/ProximaNova", "Regular"), Text::Justify::Left, 1.0f, Colour::White),
	m_textValue(this, UiBound(Vector2f(2.5f * UiInputButton::Padding.m_x, 0.5f), UiReference::CentreLeft, UiAspect::Position | UiAspect::Size), UiInputButton::FontSize, value,
		FontType::Create("Fonts/ProximaNova", "Regular"), Text::Justify::Left, 1.0f, Colour::White),
	m_soundClick("Sounds/Button1.ogg", Transform::Identity, Audio::Type::Effect, false, false, 0.9f),
	m_value(value),
	m_maxLength(maxLength),
	m_lastKey(0),
	m_updating(false),
	m_mouseOver(false)
{
	GetRectangle().SetSize(UiInputButton::Size);
	m_background.SetNinePatches(Vector4f(0.125f, 0.125f, 0.75f, 0.75f));

	Keyboard::Get()->OnKey() += [this](Key key, InputAction action, BitMask<InputMod> mods)
	{
		if (!m_updating)
		{
			return;
		}

		if (key == Key::Backspace && action != InputAction::Release)
		{
			m_inputDelay.Update(true);

			if (m_lastKey != 8 || m_inputDelay.CanInput())
			{
				m_value = m_value.substr(0, m_value.length() - 1);
				m_textValue.SetString(m_value);
				m_onValue(m_value);
				m_lastKey = 8;
			}
		}
		else if (key == Key::Enter && action != InputAction::Release && m_lastKey != 13)
		{
			m_inputDelay.Update(true);
			SetUpdating(false);
		}
	};
	Keyboard::Get()->OnChar() += [this](char c)
	{
		if (!m_updating)
		{
			return;
		}

		if (m_value.length() < static_cast<uint32_t>(m_maxLength))
		{
			m_inputDelay.Update(true);

			if (m_lastKey != c || m_inputDelay.CanInput())
			{
				m_value += c;
				m_textValue.SetString(m_value);
				m_onValue(m_value);
				m_lastKey = c;
			}
		}
		else
		{
			m_inputDelay.Update(false);
			m_lastKey = 0;
		}
	};

	OnSelected() += [this](bool selected)
	{
		Mouse::Get()->SetCursor(selected ? CursorStandard::Hand : CursorStandard::Arrow);
	};
}

void UiInputText::UpdateObject()
{
	if (Uis::Get()->WasDown(MouseButton::Left))
	{
		if (m_background.IsSelected())
		{
			SetUpdating(true);
			CancelEvent(MouseButton::Left);
		}
		else if (m_updating)
		{
			SetUpdating(false);
			CancelEvent(MouseButton::Left);
		}
	}

	if (!m_updating)
	{
		if (m_background.IsSelected() && !m_mouseOver)
		{
			m_background.SetColourDriver(new DriverSlide<Colour>(m_background.GetColourOffset(), UiInputButton::SelectedColour, UiInputButton::SlideTime));
			m_mouseOver = true;
		}
		else if (!m_background.IsSelected() && m_mouseOver)
		{
			m_background.SetColourDriver(new DriverSlide<Colour>(m_background.GetColourOffset(), UiInputButton::PrimaryColour, UiInputButton::SlideTime));
			m_mouseOver = false;
		}
	}
}

void UiInputText::SetUpdating(const bool &updating)
{
	if (!m_soundClick.IsPlaying())
	{
		m_soundClick.SetPitch(Maths::Random(0.7f, 0.9f));
		m_soundClick.Play();
	}

	m_updating = updating;
	m_mouseOver = true;
}

void UiInputText::SetValue(const std::string &value)
{
	m_value = value;
	m_textValue.SetString(value);
	//m_onValue(m_value);
}
}
