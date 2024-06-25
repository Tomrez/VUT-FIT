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

public partial class AdminSubjectManagementViewModel(
    ISubjectFacade subjectFacade,
    INavigationService navigationService,
    IMessengerService messengerService)
    : ViewModelBase(messengerService)
{
    public Guid EmptyId { get; } = Guid.Empty;
    public ObservableCollection<SubjectListModel> Subjects { get; set; } = [];

    [ObservableProperty]
    public string filterAbbreviation = null!;

    [ObservableProperty]
    public string filterName = null!;

    [ObservableProperty]
    private string sortCriteria = null!;

    public IEnumerable<SubjectListModel> FilteredSubjects
    {
        get
        {
            var filtered = Subjects.Where(a =>
                (string.IsNullOrWhiteSpace(FilterAbbreviation) || a.Abbreviation.Contains(FilterAbbreviation, StringComparison.OrdinalIgnoreCase)) &&
                (string.IsNullOrWhiteSpace(FilterName) || a.Name.Contains(FilterName, StringComparison.OrdinalIgnoreCase))
            );

            return SortSubjects(filtered);
        }
    }

    private IEnumerable<SubjectListModel> SortSubjects(IEnumerable<SubjectListModel> subjects)
    {
        return SortCriteria switch
        {
            "Abbreviation" => subjects.OrderBy(a => a.Abbreviation),
            "Name" => subjects.OrderBy(a => a.Name),
            _ => subjects
        };
    }

    protected override async Task LoadDataAsync()
    {
        await base.LoadDataAsync();

        var StudentListTemporary = await subjectFacade.GetAsync();

        Subjects.Clear();
        foreach (var curSubject in StudentListTemporary)
        {
            Subjects.Add(curSubject);
        }
    }

    [RelayCommand]
    public async Task GoBackAsync()
          => await navigationService.GoToAsync<AdminDefaultViewModel>();

    [RelayCommand]
    public async Task GoToSubjectEditAsync(Guid id)
            => await navigationService.GoToAsync<AdminSubjectEditViewModel>(
                new Dictionary<string, object?> { [nameof(AdminSubjectEditViewModel.SubjectId)] = id });

    public async void Receive(StudentLoadActivitiesMessage message)
    {
        await LoadDataAsync();
    }

    partial void OnFilterAbbreviationChanged(string value)
    {
        OnPropertyChanged(nameof(FilteredSubjects));
    }

    partial void OnFilterNameChanged(string value)
    {
        OnPropertyChanged(nameof(FilteredSubjects));
    }

    partial void OnSortCriteriaChanged(string value)
    {
        OnPropertyChanged(nameof(FilteredSubjects));
    }
}