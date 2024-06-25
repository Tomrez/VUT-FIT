using CommunityToolkit.Mvvm.Input;
using InformationSystem.App.Services;
using InformationSystem.App.ViewModels;
using InformationSystem.App.ViewModels.Admin;
using InformationSystem.App.ViewModels.Student;
using InformationSystem.App.ViewModels.Teacher;

namespace InformationSystem.App.Shells;

public partial class AppShell
{
    private readonly INavigationService _navigationService;

    public AppShell(INavigationService navigationService)
    {
        _navigationService = navigationService;

        InitializeComponent();
    }

    [RelayCommand]
    private async Task GoToStudentAsync()
        => await _navigationService.GoToAsync<StudentDefaultViewModel>();

    [RelayCommand]
    private async Task GoToAdminAsync()
        => await _navigationService.GoToAsync<AdminDefaultViewModel>();
    
    [RelayCommand]
    private async Task GoToTeacherAsync()
    => await _navigationService.GoToAsync<TeacherDefaultViewModel>();
}
