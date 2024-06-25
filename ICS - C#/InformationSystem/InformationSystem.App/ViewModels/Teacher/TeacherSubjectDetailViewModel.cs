using CommunityToolkit.Mvvm.ComponentModel;
using CommunityToolkit.Mvvm.Input;
using CommunityToolkit.Mvvm.Messaging;
using InformationSystem.App.Messages;
using InformationSystem.App.Services;
using InformationSystem.App.Views.Teacher;
using InformationSystem.BL.Facades;
using InformationSystem.BL.Models;
using System.Collections.ObjectModel;

namespace InformationSystem.App.ViewModels.Teacher;

[QueryProperty(nameof(Id), nameof(Id))]
public partial class TeacherSubjectDetailViewModel(
    ISubjectFacade subjectFacade,
    IActivityFacade activityFacade,
    INavigationService navigationService,
    IMessengerService messengerService)
    : ViewModelBase(messengerService), IRecipient<TeacherLoadSubjectsMessage>
{
    [ObservableProperty]
    public ObservableCollection<ActivityListModel> activities = [];

    [ObservableProperty]
    private Guid id = Guid.Empty;

    private SubjectDetailModel? subject;
    public SubjectDetailModel? Subject
    {
        get => subject;
        set
        {
            if (value == subject) return;
            subject = value;
            OnPropertyChanged();
        }
    }

    public IEnumerable<ActivityListModel> ActivitiesList { get; set; } = null!;

    protected override async Task LoadDataAsync()
    {
        await base.LoadDataAsync();

        Subject = await subjectFacade.GetAsync(Id);

        var allActivities = await activityFacade.GetAsync();
        ActivitiesList = allActivities.Where(a => a.SubjectId == Id);
        Activities.Clear();
        foreach (var curActivity in ActivitiesList)
        {
            Activities.Add(curActivity);
        }
    }

    public async void Receive(TeacherLoadSubjectsMessage message)
    {
        if (message.SubjectId == Subject?.Id)
        {
            await LoadDataAsync();
        }
    }

    [RelayCommand]
    public async Task GoBackAsync()
        => await navigationService.GoToAsync<TeacherDefaultViewModel>();

    [RelayCommand]
    public async Task GoToEditAsync(Guid id)
        => await navigationService.GoToAsync<TeacherEvaluationEditViewModel>(
            new Dictionary<string, object?>
            {
                [nameof(TeacherEvaluationEditViewModel.SubjectId)] = Id,
                [nameof(TeacherEvaluationEditViewModel.ActivityId)] = id
            });

    [RelayCommand]
    public async Task GoToEditActivityAsync(Guid id)
        => await navigationService.GoToAsync<TeacherSubjectActivityEditViewModel>(
            new Dictionary<string, object?>
            {
                [nameof(TeacherSubjectActivityEditViewModel.SubjectId)] = Id,
                [nameof(TeacherSubjectActivityEditViewModel.ActivityId)] = id
            });

    [RelayCommand]
    public async Task GoToActivityAddAsync(Guid id)
        => await navigationService.GoToAsync<TeacherSubjectActivityAddViewModel>(
            new Dictionary<string, object?> { [nameof(TeacherSubjectActivityAddViewModel.Id)] = id });
}
