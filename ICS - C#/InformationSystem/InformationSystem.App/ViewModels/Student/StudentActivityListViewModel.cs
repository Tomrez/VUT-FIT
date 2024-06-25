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

namespace InformationSystem.App.ViewModels.Student;

[QueryProperty(nameof(StudentId), nameof(StudentId))]
public partial class StudentActivityListViewModel(
    IStudentFacade studentFacade,
    IActivityFacade activityFacade,
    ISubjectFacade subjectFacade,
    INavigationService navigationService,
    IMessengerService messengerService)
    : ViewModelBase(messengerService), IRecipient<StudentLoadActivitiesMessage>
{
    [ObservableProperty]
    private Guid studentId = Guid.Empty;

    private StudentDetailModel? student;
    public StudentDetailModel? Student
    {
        get => student;
        set
        {
            if (value == student) return;
            student = value;
            OnPropertyChanged();
        }
    }

    public ObservableCollection<ActivityListModel> Activities { get; set; } = [];
    
    [ObservableProperty]
    public string filterSubject = null!;

    [ObservableProperty]
    public string filterActivityType = null!;

    [ObservableProperty]
    public DateTime filterStartDate = DateTime.MinValue;

    [ObservableProperty]
    public DateTime filterEndDate = DateTime.Today;

    [ObservableProperty]
    public string filterPlace = null!;

    [ObservableProperty]
    private string sortCriteria = null!;

    public IEnumerable<ActivityListModel> FilteredActivities
    {
        get
        {
            var filtered = Activities.Where(a =>
                (string.IsNullOrWhiteSpace(FilterSubject) || (a.Subject?.Abbreviation ?? string.Empty).Contains(FilterSubject, StringComparison.OrdinalIgnoreCase)) &&
                (string.IsNullOrWhiteSpace(FilterActivityType) || a.ActivityType.ToString().Contains(FilterActivityType, StringComparison.OrdinalIgnoreCase)) &&
                (FilterEndDate == DateTime.MinValue || a.ActivityEnd.Date >= FilterEndDate.Date) &&
                (FilterStartDate == DateTime.MinValue || a.ActivityStart.Date >= FilterStartDate.Date) &&
                (string.IsNullOrWhiteSpace(FilterPlace) || a.ActivityRoom.ToString().Contains(FilterPlace, StringComparison.OrdinalIgnoreCase))
            );
            return SortActivities(filtered);
        }
    }

    private IEnumerable<ActivityListModel> SortActivities(IEnumerable<ActivityListModel> activities)
    {
        return SortCriteria switch
        {
            "Subject" => activities.OrderBy(a => a.Subject?.Abbreviation),
            "Activity type" => activities.OrderBy(a => a.ActivityType),
            "End date" => activities.OrderBy(a => a.ActivityEnd),
            "Start date" => activities.OrderBy(a => a.ActivityStart),
            _ => activities
        };
    }

    public ObservableCollection<SubjectListModel> Subjects { get; set; } = [];

    protected override async Task LoadDataAsync()
    {
        await base.LoadDataAsync();

        Student = await studentFacade.GetAsync(StudentId);

        if (Student is not null)
        {
            var allSubjects = await subjectFacade.GetAsync();

            Subjects.Clear();
            foreach (var subject in allSubjects)
            {
                if (Student.StudentsSubjects.Any(a => a.SubjectId == subject.Id))
                {
                    Subjects.Add(subject);
                }
            }
        }

        var ActivitiesListTemporary = await activityFacade.GetAsync();

        Activities.Clear();
        foreach (var curActivity in ActivitiesListTemporary)
        {
            var subject = subjectFacade.GetAsync(curActivity.SubjectId);
            if (Student is null)
            {
                curActivity.Subject = await subject;
                Activities.Add(curActivity);
            }
            else
            {
                if (Subjects.Any(subj => subj.Id == curActivity.SubjectId))
                {
                    curActivity.Subject = await subject;
                    Activities.Add(curActivity);
                }
            }
        }
        OnPropertyChanged(nameof(FilteredActivities));
    }

    [RelayCommand]
    public async Task GoBackAsync()
        => await navigationService.GoToAsync<StudentDefaultViewModel>();

    public async void Receive(StudentLoadActivitiesMessage message)
    {
        await LoadDataAsync();
    }

    partial void OnFilterSubjectChanged(string value)
    {
        OnPropertyChanged(nameof(FilteredActivities));
    }

    partial void OnFilterActivityTypeChanged(string value)
    {
        OnPropertyChanged(nameof(FilteredActivities));
    }

    partial void OnFilterStartDateChanged(DateTime value)
    {
        OnPropertyChanged(nameof(FilteredActivities));
    }

    partial void OnFilterEndDateChanged(DateTime value)
    {
        OnPropertyChanged(nameof(FilteredActivities));
    }

    partial void OnFilterPlaceChanged(string value)
    {
        OnPropertyChanged(nameof(FilteredActivities));
    }

    partial void OnSortCriteriaChanged(string value)
    {
        OnPropertyChanged(nameof(FilteredActivities));
    }
}