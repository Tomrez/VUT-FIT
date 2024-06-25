using InformationSystem.App.Models;
using InformationSystem.App.ViewModels;
using InformationSystem.App.ViewModels.Admin;
using InformationSystem.App.ViewModels.Student;
using InformationSystem.App.ViewModels.Teacher;
using InformationSystem.App.Views.Admin;
using InformationSystem.App.Views.Student;
using InformationSystem.App.Views.Teacher;

namespace InformationSystem.App.Services;

public class NavigationService : INavigationService
{
    public IEnumerable<RouteModel> Routes { get; } = new List<RouteModel>
    {
        new("//studentDefault", typeof(StudentDefaultView), typeof(StudentDefaultViewModel)),
        new("//studentDefault/subjectDetail", typeof(StudentSubjectDetailView), typeof(StudentSubjectDetailViewModel)),
        new("//studentDefault/activities", typeof(StudentActivityListView), typeof(StudentActivityListViewModel)),

        new("//adminDefault", typeof(AdminDefaultView), typeof(AdminDefaultViewModel)),
        new("//adminDefault/adminPeople", typeof(AdminPeopleManagementView), typeof(AdminPeopleManagementViewModel)),
        new("//adminDefault/adminSubject", typeof(AdminSubjectManagementView), typeof(AdminSubjectManagementViewModel)),
        new("//adminDefault/adminPeopleInSubject", typeof(AdminPeopleInSubjectView), typeof(AdminPeopleInSubjectViewModel)),
        new("//adminDefault/adminPeople/adminPeopleEdit", typeof(AdminPeopleEditView), typeof(AdminPeopleEditViewModel)),
        new("//adminDefault/adminSubject/adminSubjectEdit", typeof(AdminSubjectEditView), typeof(AdminSubjectEditViewModel)),

        new("//teacherDefault", typeof(TeacherDefaultView), typeof(TeacherDefaultViewModel)),
        new("//teacherDefault/subjectDetail", typeof(TeacherSubjectDetailView), typeof(TeacherSubjectDetailViewModel)),
        new("//teacherDefault/subjectDetail/activityAdd", typeof(TeacherSubjectActivityAddView), typeof(TeacherSubjectActivityAddViewModel)),
        new("//teacherDefault/subjectDetail/activityEdit", typeof(TeacherSubjectActivityEditView), typeof(TeacherSubjectActivityEditViewModel)),
        new("//teacherDefault/subjectDetail/editEvaluation", typeof(TeacherEvaluationEditView), typeof(TeacherEvaluationEditViewModel)),
        new("//teacherDefault/activities", typeof(TeacherActivityListView), typeof(TeacherActivityListViewModel)),
        new("//teacherDefault/students", typeof(TeacherStudentsView), typeof(TeacherStudentsViewModel)),
        new("//teacherDefault/students/detail", typeof(TeacherStudentsDetailView), typeof(TeacherStudentsDetailViewModel))
    };

    public async Task GoToAsync<TViewModel>()
        where TViewModel : IViewModel
    {
        var route = GetRouteByViewModel<TViewModel>();
        await Shell.Current.GoToAsync(route);
    }
    public async Task GoToAsync<TViewModel>(IDictionary<string, object?> parameters)
        where TViewModel : IViewModel
    {
        var route = GetRouteByViewModel<TViewModel>();
        await Shell.Current.GoToAsync(route, parameters);
    }

    public async Task GoToAsync(string route)
        => await Shell.Current.GoToAsync(route);

    public async Task GoToAsync(string route, IDictionary<string, object?> parameters)
        => await Shell.Current.GoToAsync(route, parameters);

    public bool SendBackButtonPressed()
        => Shell.Current.SendBackButtonPressed();

    private string GetRouteByViewModel<TViewModel>()
        where TViewModel : IViewModel
        => Routes.First(route => route.ViewModelType == typeof(TViewModel)).Route;
}
