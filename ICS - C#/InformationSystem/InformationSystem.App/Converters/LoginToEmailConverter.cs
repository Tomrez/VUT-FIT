using System;
using CommunityToolkit.Maui.Converters;
using System.Globalization;

namespace InformationSystem.App.Converters;

public class LoginToEmailConverter : IValueConverter
{
    public object? Convert(object? value, Type targetType, object? parameter, CultureInfo? culture)
    {
        if (value is string login)
        {
            return $"{login}@vutbr.cz";
        }
        return value;
    }

    public object? ConvertBack(object? value, Type targetType, object? parameter, CultureInfo? culture)
    {
        throw new NotImplementedException();
    }
}
