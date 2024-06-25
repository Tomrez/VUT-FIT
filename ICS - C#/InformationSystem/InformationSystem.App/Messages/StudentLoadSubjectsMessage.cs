namespace InformationSystem.App.Messages;

public record StudentLoadSubjectsMessage
{
    public required Guid SubjectId { get; init; }
}
