using CommunityToolkit.Mvvm.ComponentModel;
using CommunityToolkit.Mvvm.Input;
using CommunityToolkit.Mvvm.Messaging;
using InformationSystem.App.Messages;
using InformationSystem.App.Services;
using InformationSystem.App.Views.Student;
using InformationSystem.BL.Facades;
using InformationSystem.BL.Models;
using InformationSystem.DAL.Enums;
using System.Collections.ObjectModel;
using System.Diagnostics;

namespace InformationSystem.App.ViewModels.Admin;

public partial class AdminDefaultViewModel(
    INavigationService navigationService,
    IMessengerService messengerService)
    : ViewModelBase(messengerService)
{
    [RelayCommand]
    public async Task GoToPeopleAsync()
          => await navigationService.GoToAsync<AdminPeopleManagementViewModel>();

    [RelayCommand]
    public async Task GoToSubjectAsync()
          => await navigationService.GoToAsync<AdminSubjectManagementViewModel>();

    [RelayCommand]
    public async Task GoToEditAsync()
          => await navigationService.GoToAsync<AdminPeopleInSubjectViewModel>();

    [RelayCommand]
    public void Logout()
    {
        Environment.Exit(0);
    }
}