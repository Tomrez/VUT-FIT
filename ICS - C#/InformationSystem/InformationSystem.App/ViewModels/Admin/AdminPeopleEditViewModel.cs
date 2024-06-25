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

[QueryProperty(nameof(StudentId), nameof(StudentId))]
public partial class AdminPeopleEditViewModel(
    INavigationService navigationService,
    IMessengerService messengerService,
    IStudentFacade studentFacade,
    IStudentsInSubjectFacade studentsinsubject)
    : ViewModelBase(messengerService)
{
    [ObservableProperty]
    private string name = null!;

    [ObservableProperty]
    private string surname = null!;

    [ObservableProperty]
    private string login = null!;

    [ObservableProperty]
    private string photoUrl = null!;

    [ObservableProperty]
    private Guid studentId = Guid.Empty;

    [ObservableProperty]
    public StudentDetailModel? selectedStudent = null;

    protected override async Task LoadDataAsync()
    {
        await base.LoadDataAsync();

        SelectedStudent = await studentFacade.GetAsync(StudentId);

        if (SelectedStudent is not null)
        {
            Login = SelectedStudent.Login;
            Name = SelectedStudent.Name;
            Surname = SelectedStudent.Surname;
            PhotoUrl = SelectedStudent.PhotoUrl?.ToString() ?? string.Empty;
        }
    }

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

    [RelayCommand]
    public async Task CancelAsync()
       => await navigationService.GoToAsync<AdminPeopleManagementViewModel>();

    [RelayCommand]
    public async Task AddPersonAsync()
    {
        if (Login is null or "" || Surname is null or "" || Name is null or "")
        {
            return;
        }

        var model = StudentDetailModel.Empty;

        if (StudentId == Guid.Empty)
        {
            model.Id = Guid.NewGuid();

            SelectedStudent = await studentFacade.GetLoginAsync(Login);
            if (SelectedStudent is not null)
            {
                return;
            }
        }
        else
        {
            model.Id = StudentId;
        }
        model.Login = Login;
        model.Surname = Surname;
        model.Name = Name;

        if (IsValidUri(PhotoUrl))
        {
            model.PhotoUrl = new Uri(PhotoUrl);
        }
        else
        {
            model.PhotoUrl = null;
        }

        await studentFacade.SaveAsync(model);

        await navigationService.GoToAsync<AdminPeopleManagementViewModel>();
    }

    [RelayCommand]
    public async Task DeletePersonAsync()
    {
        var existingStudent = await studentFacade.GetLoginAsync(Login);

        if (existingStudent is null)
        {
            return;
        }

        if (existingStudent.Name != Name || existingStudent.Surname != Surname)
        {
            return;
        }

        foreach (var relationship in existingStudent.StudentsSubjects)
        {
            await studentsinsubject.DeleteAsync(relationship.Id);
        }

        await studentFacade.DeleteAsync(existingStudent.Id);

        await navigationService.GoToAsync<AdminPeopleManagementViewModel>();
    }

    private bool IsValidUri(string uriString)
    {
        return Uri.TryCreate(uriString, UriKind.Absolute, out _);
    }
}
