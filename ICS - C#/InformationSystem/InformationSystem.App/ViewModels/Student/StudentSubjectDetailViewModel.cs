using CommunityToolkit.Mvvm.ComponentModel;
using CommunityToolkit.Mvvm.Input;
using CommunityToolkit.Mvvm.Messaging;
using InformationSystem.App.Messages;
using InformationSystem.App.Services;
using InformationSystem.App.Views.Student;
using InformationSystem.BL.Facades;
using InformationSystem.BL.Models;
using System.Collections.ObjectModel;

namespace InformationSystem.App.ViewModels.Student;

[QueryProperty(nameof(Id), nameof(Id))]
[QueryProperty(nameof(StudentId), nameof(StudentId))]
public partial class StudentSubjectDetailViewModel(
    IStudentFacade studentFacade,
    ISubjectFacade subjectFacade,
    IActivityFacade activityFacade,
    IStudentEvaluationFacade studentEvaluationFacade,
    INavigationService navigationService,
    IMessengerService messengerService)
    : ViewModelBase(messengerService), IRecipient<StudentLoadSubjectsMessage>, IRecipient<TeacherEvaluationAddMessage>,
                                       IRecipient<TeacherEvaluationEditMessage>, IRecipient<TeacherEvaluationDeleteMessage>
{
    [ObservableProperty]
    private uint pointsSum = 0;

    [ObservableProperty]
    private string sortCriteria = null!;

    public IEnumerable<ActivityAndEvaluationListModel> SortedActivitiesWithEvaluations
    {
        get
        {
            var sorted = ActivitiesWithEvaluations;
            return SortActivities(sorted);
        }
    }

    private IEnumerable<ActivityAndEvaluationListModel> SortActivities(IEnumerable<ActivityAndEvaluationListModel> activitiesWithEvals)
    {
        return SortCriteria switch
        {
            "Activity type" => activitiesWithEvals.OrderBy(a => a.Subject?.Abbreviation),
            "End date" => activitiesWithEvals.OrderBy(a => a.ActivityEnd),
            "Points" => activitiesWithEvals.OrderBy(a => a.ActivityPoints),
            _ => activitiesWithEvals
        };
    }

    [ObservableProperty]
    public ObservableCollection<ActivityAndEvaluationListModel> activitiesWithEvaluations = [];

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

    protected override async Task LoadDataAsync()
    {
        await base.LoadDataAsync();

        Student = await studentFacade.GetAsync(StudentId);
        Subject = await subjectFacade.GetAsync(Id);

        var allActivities = await activityFacade.GetAsync();
        var subjectActivities = allActivities.Where(a => a.SubjectId == Id);

        ActivitiesWithEvaluations.Clear();
        PointsSum = 0;

        foreach (var curActivity in subjectActivities)
        {
            var allEvaluations = await studentEvaluationFacade.GetAsync();
            var activityEvaluation = allEvaluations.Where(a => a.ActivityId == curActivity.Id);

            if (Student != null)
            {
                activityEvaluation = Student.StudentEvaluations.Where(stud => stud.ActivityId == curActivity.Id);
            }

            if (activityEvaluation.Any())
            {
                foreach (var curEvaluation in activityEvaluation)
                {
                    ActivityAndEvaluationListModel activityWithEvaluation = new()
                    {
                        SubjectId = curActivity.SubjectId,
                        ActivityType = curActivity.ActivityType,
                        ActivityEnd = curActivity.ActivityEnd,
                        ActivityPoints = curEvaluation.ActivityPoints,
                        ActivityEvaluator = curEvaluation.ActivityEvaluator,
                        ActivityNotes = curEvaluation.ActivityNotes
                    };
                    PointsSum += activityWithEvaluation.ActivityPoints;

                    ActivitiesWithEvaluations.Add(activityWithEvaluation);
                }
            }
        }
    }

    public async void Receive(StudentLoadSubjectsMessage message)
    {
        if (message.SubjectId == Subject?.Id)
        {
            await LoadDataAsync();
        }
    }

    public async void Receive(TeacherEvaluationAddMessage message)
    {
        await LoadDataAsync();
    }

    public async void Receive(TeacherEvaluationEditMessage message)
    {
        await LoadDataAsync();
    }

    public async void Receive(TeacherEvaluationDeleteMessage message)
    {
        await LoadDataAsync();
    }

    [RelayCommand]
    public async Task GoBackAsync()
        => await navigationService.GoToAsync<StudentDefaultViewModel>();


    partial void OnSortCriteriaChanged(string value)
    {
        OnPropertyChanged(nameof(SortedActivitiesWithEvaluations));
    }
}