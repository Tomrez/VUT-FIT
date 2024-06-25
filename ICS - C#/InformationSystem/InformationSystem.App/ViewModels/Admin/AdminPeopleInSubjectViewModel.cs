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
using System.Xml.Linq;
using System.ComponentModel;
using InformationSystem.BL.Mappers;

namespace InformationSystem.App.ViewModels.Admin;

public partial class AdminPeopleInSubjectViewModel(
    IStudentFacade studentFacade,
    ISubjectFacade subjectFacade,
    IStudentsInSubjectFacade studentsinsubject,
    INavigationService navigationService,
    IMessengerService messengerService)
    : ViewModelBase(messengerService)
{
    [ObservableProperty]
    public StudentDetailModel? student = null;

    [ObservableProperty]
    public ObservableCollection<SubjectListModel> subjectList = [];

    [ObservableProperty]
    private string login = null!;

    [ObservableProperty]
    private string name = null!;
   
    [ObservableProperty]
    private StudentListModel? selectedStudent;

    [ObservableProperty]
    private SubjectListModel? selectedSubject;

    [ObservableProperty]
    public ObservableCollection<SubjectListModel> subjects = new();

    [ObservableProperty]
    public ObservableCollection<StudentListModel> students = new();

    protected override async Task LoadDataAsync()
    {
        await base.LoadDataAsync();

        var SubjectsList = await subjectFacade.GetAsync();

        Subjects.Clear();

        foreach (var curSubject in SubjectsList)
        {
            Subjects.Add(curSubject);
        }

        var StudentList = await studentFacade.GetAsync();

        Students.Clear();

        foreach (var curStudent in StudentList)
        {
            Students.Add(curStudent);
        }
    }

    partial void OnSelectedStudentChanged(StudentListModel? value)
    {
        if (value != null)
        {
            GetStudentAsync(value.Id);
            if (Student is not null)
            {
                var SubjectsList = Student.StudentsSubjects;
                SubjectList.Clear();
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
            SubjectList.Add(subjectToAddList);
        }
    }

    [RelayCommand]
    public async Task CancelAsync()
       => await navigationService.GoToAsync<AdminDefaultViewModel>();

    [RelayCommand]
    public async Task AddPersonAsync()
    {

        if (SelectedStudent is null || SelectedSubject is null)
        {
            return;
        }

        var listModel = new StudentsInSubjectListModel
        {
            SubjectId = SelectedSubject.Id,
            SubjectName = SelectedSubject.Name,
            SubjectAbbreviation = SelectedSubject.Abbreviation
        };

        StudentDetailModel? student = await studentFacade.GetAsync(SelectedStudent.Id);

        if (student is not null)
        {
            if (student.StudentsSubjects.Any(ss => ss.SubjectId == listModel.SubjectId))
            {
                return;
            }

            student.StudentsSubjects.Add(listModel);
        }

        var detailModel = new StudentsInSubjectDetailModel
        {
            SubjectId = SelectedSubject.Id,
            SubjectName = SelectedSubject.Name,
            SubjectAbbreviation = SelectedSubject.Abbreviation
        };

        await studentsinsubject.SaveAsync(detailModel, SelectedStudent.Id);
        await navigationService.GoToAsync<AdminDefaultViewModel>();
    }

    [RelayCommand]
    public async Task DeletePersonAsync()
    {
        if (SelectedStudent is null || SelectedSubject is null)
        {
            return;
        }

        StudentDetailModel? student = await studentFacade.GetAsync(SelectedStudent.Id);

        if (student is not null)
        {
            var relationship = student.StudentsSubjects.FirstOrDefault(ss => ss.SubjectId == SelectedSubject.Id);

            if (relationship == null)
            {
                return;
            }

            student.StudentsSubjects.Remove(relationship);

            await studentsinsubject.DeleteAsync(relationship.Id);
        }
        await navigationService.GoToAsync<AdminDefaultViewModel>();
    }
}
