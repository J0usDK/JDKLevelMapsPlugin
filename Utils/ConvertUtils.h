#pragma once
#include <CryCore/BaseTypes.h>
#include <CrySystem/ISystem.h>
#include <QVariant>

namespace JDKLevelMaps::Utils::ConvertUtils
{
	inline uint8 QVariantToUint8(const QVariant& value, const uint8& defaultValue = 0)
	{
		if (!value.isValid())
			return defaultValue;

		bool isOk = false;
		uint tempValue = value.toUInt(&isOk);

		uint8 ret = defaultValue;

		if (isOk && tempValue <= 255)
			ret = static_cast<uint8>(tempValue);
		else
			CryWarning(VALIDATOR_MODULE_EDITOR, VALIDATOR_WARNING, "[JDKLevelMaps] Cannot convert value to uint8");

		return ret;
	}

	inline float QVariantToFloat(const QVariant& value, const float& defaultValue = 0.0f)
	{
		if (!value.isValid())
			return defaultValue;

		bool isOk = false;
		float tempValue = value.toFloat(&isOk);

		float ret = defaultValue;

		if (isOk)
			ret = tempValue;
		else
			CryWarning(VALIDATOR_MODULE_EDITOR, VALIDATOR_WARNING, "[JDKLevelMaps] Cannot convert value to float");

		return ret;
	}

	inline std::string QVariantToStdString(const QVariant& value, const std::string& defaultValue = "")
	{
		if (!value.isValid())
			return defaultValue;

		QString qret = value.toString();
		std::string ret = defaultValue;

		if (!qret.isEmpty())
			ret = qret.toStdString();
		else
			CryWarning(VALIDATOR_MODULE_EDITOR, VALIDATOR_WARNING, "[JDKLevelMaps] Cannot convert value to std::string");

		return ret;
	}

	inline bool QVariantToBool(const QVariant& value, const bool& defaultValue = false)
	{
		if (!value.isValid())
			return defaultValue;

		bool ret = defaultValue;

		if (value.type() == QMetaType::Bool)
			ret = value.toBool();
		else
			CryWarning(VALIDATOR_MODULE_EDITOR, VALIDATOR_WARNING, "[JDKLevelMaps] Cannot convert value to bool");

		return ret;
	}
}