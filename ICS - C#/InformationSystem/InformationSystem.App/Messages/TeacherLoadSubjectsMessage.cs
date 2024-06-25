namespace InformationSystem.App.Messages;

public record TeacherLoadSubjectsMessage
{
    public required Guid SubjectId { get; init; }
}
