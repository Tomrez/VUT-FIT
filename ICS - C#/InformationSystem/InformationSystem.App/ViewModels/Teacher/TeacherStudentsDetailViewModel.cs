using CommunityToolkit.Mvvm.ComponentModel;
using CommunityToolkit.Mvvm.Input;
using CommunityToolkit.Mvvm.Messaging;
using InformationSystem.App.Messages;
using InformationSystem.App.Services;
using InformationSystem.App.Views.Teacher;
using InformationSystem.BL.Facades;
using InformationSystem.BL.Mappers;
using InformationSystem.BL.Models;
using InformationSystem.DAL.UnitOfWork;
using System.Collections.ObjectModel;

namespace InformationSystem.App.ViewModels.Teacher;

[QueryProperty(nameof(Id), nameof(Id))]
public partial class TeacherStudentsDetailViewModel(
    IStudentFacade studentFacade,
    INavigationService navigationService,
    IMessengerService messengerService)
    : ViewModelBase(messengerService)
{
    [ObservableProperty]
    public string? studentLogin = null!;

    [ObservableProperty]
    public string? studentName = null!;

    [ObservableProperty]
    private string? studentSurname = null!;

    [ObservableProperty]
    private Uri? studentPhotoUrl = null;

    [ObservableProperty]
    public string fullName = null!;

    [ObservableProperty]
    private Guid id = Guid.Empty;

    public StudentDetailModel? studentDetail;
    public StudentDetailModel? StudentDetail
    {
        get => studentDetail;
        set
        {
            if (value == studentDetail) return;
            studentDetail = value;
            OnPropertyChanged();
        }
    }

    protected override async Task LoadDataAsync()
    {
        await base.LoadDataAsync();

        StudentDetail = await studentFacade.GetAsync(Id);

        StudentLogin = StudentDetail?.Login;
        StudentName = StudentDetail?.Name;
        StudentSurname = StudentDetail?.Surname;
        StudentPhotoUrl = StudentDetail?.PhotoUrl;

        FullName = $"{StudentName} {StudentSurname}";

        if (StudentPhotoUrl is null)
        {
            StudentPhotoUrl = new Uri("https://avatars.githubusercontent.com/u/9011267?v=4");
        }
    }

    [RelayCommand]
    public async Task GoBackAsync()
        => await navigationService.GoToAsync<TeacherStudentsViewModel>();
}
