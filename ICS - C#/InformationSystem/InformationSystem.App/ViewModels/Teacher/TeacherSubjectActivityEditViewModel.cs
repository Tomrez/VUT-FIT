using CommunityToolkit.Mvvm.ComponentModel;
using CommunityToolkit.Mvvm.Input;
using CommunityToolkit.Mvvm.Messaging;
using InformationSystem.App.Messages;
using InformationSystem.App.Services;
using InformationSystem.App.Views.Teacher;
using InformationSystem.BL.Facades;
using InformationSystem.BL.Models;
using System.Collections.ObjectModel;
using InformationSystem.DAL.Enums;
using InformationSystem.DAL.Seeds;
using InformationSystem.BL.Mappers;

namespace InformationSystem.App.ViewModels.Teacher;

[QueryProperty(nameof(SubjectId), nameof(SubjectId))]
[QueryProperty(nameof(ActivityId), nameof(ActivityId))]
public partial class TeacherSubjectActivityEditViewModel(

    IActivityFacade activityFacade,
    IStudentEvaluationFacade studentEvaluationFacade,
    IStudentFacade studentFacade,
    INavigationService navigationService,
    IMessengerService messengerService)
    : ViewModelBase(messengerService)
{
    [ObservableProperty]
    private Guid subjectId = Guid.Empty;

    [ObservableProperty]
    private Guid activityId = Guid.Empty;

    [ObservableProperty]
    private ActivityType newActivityType = ActivityType.None;

    [ObservableProperty]
    private DateTime startDate = DateTime.Today;

    [ObservableProperty]
    private DateTime startTime = DateTime.UtcNow;

    [ObservableProperty]
    private DateTime endDate = DateTime.Today;

    [ObservableProperty]
    private Room newRoom = Room.None;

    [ObservableProperty]
    private string? newDescription = null!;

    [ObservableProperty]
    public ActivityType[] activityTypes = (ActivityType[])Enum.GetValues(typeof(ActivityType));

    [ObservableProperty]
    public Room[] rooms = (Room[])Enum.GetValues(typeof(Room));

    [ObservableProperty]
    public ActivityDetailModel? loadActivity = ActivityDetailModel.Empty;

    protected override async Task LoadDataAsync()
    {
        await base.LoadDataAsync();

        LoadActivity = await activityFacade.GetAsync(ActivityId);
        if (LoadActivity is null)
        {
            return;
        }

        NewActivityType = LoadActivity.ActivityType;
        StartDate = LoadActivity.ActivityStart;
        EndDate = LoadActivity.ActivityEnd;
        NewRoom = LoadActivity.Room;
        NewDescription = LoadActivity.Description;
    }

    [RelayCommand]
    public async Task GoToDetailAsync()
        => await navigationService.GoToAsync<TeacherSubjectDetailViewModel>(
            new Dictionary<string, object?> { [nameof(TeacherSubjectActivityAddViewModel.Id)] = SubjectId });

    [RelayCommand]
    public async Task EditActivityAsync()
    {
        if (NewRoom != Room.None && NewActivityType != ActivityType.None)
        {

            var StartDateTime = new DateTime(
                StartDate.Year,
                StartDate.Month,
                StartDate.Day,
                StartTime.Hour,
                StartTime.Minute,
                StartTime.Second
                );

            var EndDateTime = new DateTime(
                EndDate.Year,
                EndDate.Month,
                EndDate.Day,
                StartTime.Hour + 1,
                StartTime.Minute,
                StartTime.Second
                );

            var activity = ActivityDetailModel.Empty;

            activity.SubjectId = SubjectId;
            activity.ActivityStart = StartDateTime;
            activity.ActivityEnd = EndDateTime;
            activity.Description = NewDescription;
            activity.Room = NewRoom;
            activity.ActivityType = NewActivityType;
            activity.Id = ActivityId;

            await activityFacade.SaveAsync(activity);
        }
        await navigationService.GoToAsync<TeacherSubjectDetailViewModel>(
            new Dictionary<string, object?> { [nameof(TeacherSubjectActivityAddViewModel.Id)] = SubjectId });
    }

    [RelayCommand]
    public async Task DeleteActivityAsync()
    {
        var students = await studentFacade.GetAsync();
        foreach (var student in students)
        {
            var student2 = await studentFacade.GetAsync(student.Id);
            var evaluationsToDelete = student2!.StudentEvaluations.Where(eval => eval.ActivityId == ActivityId);
            foreach (var evaluation in evaluationsToDelete)
            {
                await studentEvaluationFacade.DeleteAsync(evaluation.Id);
                MessengerService.Send(new TeacherEvaluationDeleteMessage());
            }
        }
        await activityFacade.DeleteAsync(ActivityId);

        await navigationService.GoToAsync<TeacherSubjectDetailViewModel>(
            new Dictionary<string, object?> { [nameof(TeacherSubjectActivityAddViewModel.Id)] = SubjectId });
    }

    public async void Receive(TeacherSubjectActivityAddMessage message)
    {
        await LoadDataAsync();
    }
}
