using CommunityToolkit.Mvvm.ComponentModel;
using CommunityToolkit.Mvvm.Input;
using CommunityToolkit.Mvvm.Messaging;
using InformationSystem.App.Messages;
using InformationSystem.App.Services;
using InformationSystem.BL.Facades;
using InformationSystem.BL.Models;
using InformationSystem.DAL.Enums;
using System.Collections.ObjectModel;
using System.Threading.Tasks;
using System.Diagnostics;
using System.ComponentModel.DataAnnotations;
using InformationSystem.BL.Mappers;

namespace InformationSystem.App.ViewModels.Teacher;

[QueryProperty(nameof(SubjectId), nameof(SubjectId))]
[QueryProperty(nameof(ActivityId), nameof(ActivityId))]
public partial class TeacherEvaluationEditViewModel(
    IStudentFacade studentFacade,
    IStudentEvaluationFacade studentEvaluationFacade,
    IActivityFacade activityFacade,
    StudentEvaluationModelMapper studentEvaluationModelMapper,
    INavigationService navigationService,
    IMessengerService messengerService)
    : ViewModelBase(messengerService), IRecipient<TeacherEvaluationAddMessage>, IRecipient<TeacherEvaluationEditMessage>,
                                       IRecipient<TeacherEvaluationDeleteMessage>
{
    [ObservableProperty]
    public StudentDetailModel? student = null;

    [ObservableProperty]
    private Guid subjectId = Guid.Empty;

    [ObservableProperty]
    private Guid activityId = Guid.Empty;

    [ObservableProperty]
    public ObservableCollection<StudentListModel> students = [];

    [ObservableProperty]
    public StudentListModel? selectedStudent = null;

    [ObservableProperty]
    public string points = string.Empty;

    [ObservableProperty]
    public string note = string.Empty;

    private ActivityDetailModel? activity;
    public ActivityDetailModel? Activity
    {
        get => activity;
        set
        {
            if (value == activity) return;
            activity = value;
            OnPropertyChanged();
        }
    }

    private StudentEvaluationListModel? Evaluation = null;

    public StudentEvaluationDetailModel? EvaluationNew { get; private set; }

    protected override async Task LoadDataAsync()
    {
        await base.LoadDataAsync();

        Activity = await activityFacade.GetAsync(ActivityId);

        var StudentsList = await studentFacade.GetAsync();

        Students.Clear();
        foreach (var student in StudentsList)
        {
            var student2 = await studentFacade.GetAsync(student.Id);
            if (student2 is not null)
            {
                if (student2.StudentsSubjects.Any(subj => subj.SubjectId == SubjectId))
                {
                    Students.Add(student);
                }
            }
        }
    }

    [RelayCommand]
    public async Task CancelAsync()
        => await navigationService.GoToAsync<TeacherSubjectDetailViewModel>(
            new Dictionary<string, object?> { [nameof(TeacherSubjectDetailViewModel.Id)] = SubjectId });

    [RelayCommand]
    public async Task EditAsync()
    {
        if (Points == string.Empty)
        {
            if (Evaluation is not null)
            {
                await studentEvaluationFacade.DeleteAsync(Evaluation.Id);
                MessengerService.Send(new TeacherEvaluationDeleteMessage());
            }
        }
        else
        {
            EvaluationNew = new()
            {
                Id = Guid.NewGuid(),
                ActivityId = ActivityId,
                ActivityPoints = uint.Parse(Points),
                ActivityNotes = Note,
                ActivityEvaluator = "System"
            };

            if (Evaluation is not null)
            {
                await studentEvaluationFacade.SaveAsync(EvaluationNew, SelectedStudent!.Id);
                SelectedStudent.StudentEvaluations.Add(studentEvaluationModelMapper.MapToListModel(EvaluationNew));
                MessengerService.Send(new TeacherEvaluationEditMessage());

                await studentEvaluationFacade.DeleteAsync(Evaluation.Id);
                MessengerService.Send(new TeacherEvaluationDeleteMessage());
            }
            else
            {
                await studentEvaluationFacade.SaveAsync(EvaluationNew, SelectedStudent!.Id);
                SelectedStudent.StudentEvaluations.Add(studentEvaluationModelMapper.MapToListModel(EvaluationNew));
                MessengerService.Send(new TeacherEvaluationAddMessage());
            }
        }
        await navigationService.GoToAsync<TeacherSubjectDetailViewModel>(
            new Dictionary<string, object?> { [nameof(TeacherSubjectDetailViewModel.Id)] = SubjectId });
    }

    partial void OnSelectedStudentChanged(StudentListModel? value)
    {
        if (value != null)
        {
            GetStudentAsync(value.Id);
            if (Student is not null)
            {
                Evaluation = Student.StudentEvaluations.Where(eval => eval.ActivityId == ActivityId).FirstOrDefault();
                Points = Evaluation?.ActivityPoints.ToString() ?? string.Empty;
                Note = Evaluation?.ActivityNotes ?? string.Empty;
            }
        }
        OnPropertyChanged(nameof(Points));
        OnPropertyChanged(nameof(Note));
    }

    private async void GetStudentAsync(Guid studentId)
    {
        Student = await studentFacade.GetAsync(studentId);
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
}
