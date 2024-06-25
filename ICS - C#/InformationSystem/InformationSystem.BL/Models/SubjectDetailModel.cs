using InformationSystem.DAL.Enums;
using System.Collections.ObjectModel;

namespace InformationSystem.BL.Models;

public record SubjectDetailModel : ModelBase
{
    public required string Name { get; set; }
    public required string Abbreviation { get; set; }
    public ObservableCollection<ActivityListModel> Activities { get; init; } = new();

    public static SubjectDetailModel Empty => new()
    {
        Id = Guid.NewGuid(),
        Name = string.Empty,
        Abbreviation = string.Empty
    };
}