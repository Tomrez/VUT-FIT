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


namespace InformationSystem.App.ViewModels.Admin;

[QueryProperty(nameof(SubjectId), nameof(SubjectId))]
public partial class AdminSubjectEditViewModel(
    IStudentFacade studentFacade,
    IStudentsInSubjectFacade studentsInSubjectFacade,
    ISubjectFacade subjectFacade,
    IActivityFacade activityFacade,
    INavigationService navigationService,
    IMessengerService messengerService)
    : ViewModelBase(messengerService)
{
    [ObservableProperty]
    private string abbreviation = null!;

    [ObservableProperty]
    private string name = null!;

    [ObservableProperty]
    private Guid subjectId = Guid.Empty;

    [ObservableProperty]
    public SubjectDetailModel? selectedSubject = null;

    protected override async Task LoadDataAsync()
    {
        await base.LoadDataAsync();

        SelectedSubject = await subjectFacade.GetAsync(SubjectId);

        if (SelectedSubject is not null)
        {
            Abbreviation = SelectedSubject.Abbreviation;
            Name = SelectedSubject.Name;
            Abbreviation = SelectedSubject.Abbreviation;
        }
    }

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

    [RelayCommand]
    public async Task CancelAsync()
       => await navigationService.GoToAsync<AdminSubjectManagementViewModel>();

    [RelayCommand]
    public async Task AddSubjectAsync()
    {
        if (Name is null or "" || Abbreviation is null or "")
        {
            return;
        }

        var model = SubjectDetailModel.Empty;

        if (SubjectId == Guid.Empty)
        {
            model.Id = Guid.NewGuid();
        }
        else
        {
            model.Id = SubjectId;
        }
        model.Name = Name;
        model.Abbreviation = Abbreviation;

        await subjectFacade.SaveAsync(model);

        await navigationService.GoToAsync<AdminSubjectManagementViewModel>();
    }

    [RelayCommand]
    public async Task DeleteSubjectAsync()
    {
        var activities = await activityFacade.GetAsync();

        var students = await studentFacade.GetAsync();
        foreach (var student in students)
        {
            var student2 = await studentFacade.GetAsync(student.Id);
            var subjectToDelete = student2!.StudentsSubjects.Where(eval => eval.SubjectId == SubjectId);
            foreach (var subject in subjectToDelete)
            {
                foreach (var activity in activities)
                {
                    if (activity.SubjectId == subject.Id)
                    {
                        await activityFacade.DeleteAsync(activity.Id);
                    }
                }
                await studentsInSubjectFacade.DeleteAsync(subject.Id);
                MessengerService.Send(new AdminSubjectEditMessage());
            }
        }

        await subjectFacade.DeleteAsync(SubjectId);

        await navigationService.GoToAsync<AdminSubjectManagementViewModel>();
    }
}
