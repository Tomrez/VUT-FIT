using CommunityToolkit.Mvvm.ComponentModel;
using CommunityToolkit.Mvvm.Input;
using CommunityToolkit.Mvvm.Messaging;
using InformationSystem.App.Messages;
using InformationSystem.App.Services;
using InformationSystem.App.Views.Student;
using InformationSystem.BL.Facades;
using InformationSystem.BL.Models;
using System.Collections.ObjectModel;
using System.Xml.Linq;

namespace InformationSystem.App.ViewModels.Student;

public partial class StudentDefaultViewModel(
    IStudentFacade studentFacade,
    ISubjectFacade subjectFacade,
    INavigationService navigationService,
    IMessengerService messengerService)
    : ViewModelBase(messengerService), IRecipient<StudentLoadSubjectsMessage>, IRecipient<AdminSubjectEditMessage>
{
    [ObservableProperty]
    public StudentDetailModel? student = null;

    [ObservableProperty]
    public ObservableCollection<StudentListModel> students = [];

    [ObservableProperty]
    public ObservableCollection<SubjectListModel> subjects = [];

    [ObservableProperty]
    public StudentListModel? selectedStudent = null;

    protected override async Task LoadDataAsync()
    {
        await base.LoadDataAsync();

        var StudentsList = await studentFacade.GetAsync();
        Students.Clear();
        foreach (var student in StudentsList)
        {
            Students.Add(student);
        }
    }

    [RelayCommand]
    public async Task GoToDetailAsync(Guid id)
        => await navigationService.GoToAsync<StudentSubjectDetailViewModel>(
            new Dictionary<string, object?> {
                [nameof(StudentSubjectDetailViewModel.Id)] = id,
                [nameof(StudentActivityListViewModel.StudentId)] = (SelectedStudent?.Id ?? Guid.Empty)
            });

    [RelayCommand]
    public async Task GoToActivitiesAsync()
    {
        if (SelectedStudent is not null)
        {
            await navigationService.GoToAsync<StudentActivityListViewModel>(
                new Dictionary<string, object?> { [nameof(StudentActivityListViewModel.StudentId)] = SelectedStudent.Id });
        }
    }

    public async void Receive(StudentLoadSubjectsMessage message)
    {
        await LoadDataAsync();
    }

    public async void Receive(AdminSubjectEditMessage message)
    {
        await LoadDataAsync();
    }

    partial void OnSelectedStudentChanged(StudentListModel? value)
    {
        if (value != null)
        {
            GetStudentAsync(value.Id);
            if (Student is not null)
            {
                var SubjectsList = Student.StudentsSubjects;
                Subjects.Clear();
                foreach (var curSubject in SubjectsList)
                {
                    AddSubjectToCollectionAsync(curSubject.SubjectId);
                }
            }
        }
        OnPropertyChanged(nameof(Subjects));
    }

    private async void GetStudentAsync(Guid studentId)
    {
        Student = await studentFacade.GetAsync(studentId);
    }

    private async void AddSubjectToCollectionAsync(Guid subjectId)
    {
        var subjectToAdd = await subjectFacade.GetAsync(subjectId);
        if (subjectToAdd != null)
        {
            SubjectListModel subjectToAddList = new()
            {
                Id = subjectToAdd.Id,
                Abbreviation = subjectToAdd.Abbreviation,
                Name = subjectToAdd.Name
            };
            Subjects.Add(subjectToAddList);
        }
    }

    [RelayCommand]
    public void Logout()
    {
        Environment.Exit(0);
    }
}