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

namespace InformationSystem.App.ViewModels.Admin;

public partial class AdminPeopleManagementViewModel(
    IStudentFacade studentFacade,
    INavigationService navigationService,
    IMessengerService messengerService)
    : ViewModelBase(messengerService)
{
    public Guid EmptyId { get; } = Guid.Empty;
    public ObservableCollection<StudentListModel> Students { get; set; } = [];

    [ObservableProperty]
    public string filterName = null!;

    [ObservableProperty]
    public string filterSurname = null!;

    [ObservableProperty]
    public string filterLogin = null!;

    [ObservableProperty]
    private string sortCriteria = null!;

    public IEnumerable<StudentListModel> FilteredStudents
    {
        get
        {
            var filtered = Students.Where(a =>
                (string.IsNullOrWhiteSpace(FilterName) || a.Name.Contains(FilterName, StringComparison.OrdinalIgnoreCase)) &&
                (string.IsNullOrWhiteSpace(FilterSurname) || a.Surname.Contains(FilterSurname, StringComparison.OrdinalIgnoreCase)) &&
                (string.IsNullOrWhiteSpace(FilterLogin) || a.Login.Contains(FilterLogin, StringComparison.OrdinalIgnoreCase))
            );

             return SortStudents(filtered);
        }
    }

    private IEnumerable<StudentListModel> SortStudents(IEnumerable<StudentListModel> students)
    {
        return SortCriteria switch
        {
            "Name" => students.OrderBy(a => a.Name),
            "Surname" => students.OrderBy(a => a.Surname),
            "Login" => students.OrderBy(a => a.Login),
            _ => students
        };
    }

    protected override async Task LoadDataAsync()
    {
        await base.LoadDataAsync();

        var StudentListTemporary = await studentFacade.GetAsync();

        Students.Clear();
        foreach (var curStudent in StudentListTemporary)
        {
            Students.Add(curStudent);
        }
    }

    [RelayCommand]
    public async Task GoBackAsync()
          => await navigationService.GoToAsync<AdminDefaultViewModel>();


    [RelayCommand]
    public async Task GoToPersonEditAsync(Guid id)
            => await navigationService.GoToAsync<AdminPeopleEditViewModel>(
                new Dictionary<string, object?> { [nameof(AdminPeopleEditViewModel.StudentId)] = id });

    public async void Receive(StudentLoadActivitiesMessage message)
    {
        await LoadDataAsync();
    }

    partial void OnFilterNameChanged(string value)
    {
        OnPropertyChanged(nameof(FilteredStudents));
    }

    partial void OnFilterSurnameChanged(string value)
    {
        OnPropertyChanged(nameof(FilteredStudents));
    }

    partial void OnFilterLoginChanged(string value)
    {
        OnPropertyChanged(nameof(FilteredStudents));
    }

    partial void OnSortCriteriaChanged(string value)
    {
        OnPropertyChanged(nameof(FilteredStudents));
    }
}