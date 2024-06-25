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

namespace InformationSystem.App.ViewModels.Teacher;

[QueryProperty(nameof(Id), nameof(Id))]
public partial class TeacherSubjectActivityAddViewModel(

    IActivityFacade activityFacade,
    INavigationService navigationService,
    IMessengerService messengerService)
    : ViewModelBase(messengerService), IRecipient<TeacherSubjectActivityAddMessage>
{
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
    private string newDescription = null!;

    [ObservableProperty]
    private Guid id = Guid.Empty;

    [ObservableProperty]
    public ActivityType[] activityTypes = (ActivityType[])Enum.GetValues(typeof(ActivityType));

    [ObservableProperty]
    public Room[] rooms = (Room[])Enum.GetValues(typeof(Room));

    [RelayCommand]
    public async Task GoToDetailAsync()
        => await navigationService.GoToAsync<TeacherSubjectDetailViewModel>(
            new Dictionary<string, object?> { [nameof(TeacherSubjectActivityAddViewModel.Id)] = Id });

    [RelayCommand]
    public async Task AddPersonAsync()
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

            var activity = new ActivityDetailModel()
            {
                SubjectId = Id,
                ActivityStart = StartDateTime,
                ActivityEnd = EndDateTime,
                Description = NewDescription,
                Room = NewRoom,
                ActivityType = NewActivityType
            };

            await activityFacade.SaveAsync(activity);

            await navigationService.GoToAsync<TeacherSubjectDetailViewModel>(
                new Dictionary<string, object?> { [nameof(TeacherSubjectActivityAddViewModel.Id)] = Id });
        }
    }

    public async void Receive(TeacherSubjectActivityAddMessage message)
    {
        await LoadDataAsync();
    }

    partial void OnStartTimeChanged(DateTime value)
    {
        OnPropertyChanged(nameof(StartDate));
        OnPropertyChanged(nameof(StartTime));
    }
}
