using CommunityToolkit.Mvvm.ComponentModel;
using CommunityToolkit.Mvvm.Input;
using CommunityToolkit.Mvvm.Messaging;
using InformationSystem.App.Messages;
using InformationSystem.App.Services;
using InformationSystem.App.Views.Teacher;
using InformationSystem.BL.Facades;
using InformationSystem.BL.Models;
using System.Collections.ObjectModel;
using System.Xml.Linq;

namespace InformationSystem.App.ViewModels.Teacher;

public partial class TeacherDefaultViewModel(
    ISubjectFacade subjectFacade,
    INavigationService navigationService,
    IMessengerService messengerService)
    : ViewModelBase(messengerService), IRecipient<TeacherLoadSubjectsMessage>
{
    [ObservableProperty]
    public ObservableCollection<SubjectListModel> subjects = [];

    public IEnumerable<SubjectListModel> SubjectsList { get; set; } = null!;
    protected override async Task LoadDataAsync()
    {
        await base.LoadDataAsync();

        SubjectsList = await subjectFacade.GetAsync();

        Subjects.Clear();
        foreach (var curSubject in SubjectsList)
        {
            Subjects.Add(curSubject);
        }
    }

    [RelayCommand]
    public async Task GoToDetailAsync(Guid id)
        => await navigationService.GoToAsync<TeacherSubjectDetailViewModel>(
            new Dictionary<string, object?> { [nameof(TeacherSubjectDetailViewModel.Id)] = id });

    [RelayCommand]
    public async Task GoToActivitiesAsync()
        => await navigationService.GoToAsync<TeacherActivityListViewModel>();

    [RelayCommand]
    public async Task GoToStudentsAsync()
        => await navigationService.GoToAsync<TeacherStudentsViewModel>();

    public async void Receive(TeacherLoadSubjectsMessage message)
    {
        await LoadDataAsync();
    }

    [RelayCommand]
    public void Logout()
    {
        Environment.Exit(0);
    }
}