using InformationSystem.BL.Models;
using InformationSystem.DAL.Entities;

namespace InformationSystem.BL.Mappers;

public class StudentsInSubjectModelMapper :
    ModelBaseMapper<StudentsInSubjectEntity, StudentsInSubjectListModel, StudentsInSubjectDetailModel>
{
    public override StudentsInSubjectListModel MapToListModel(StudentsInSubjectEntity? entity)
        => entity?.Student is null
            ? StudentsInSubjectListModel.Empty
            : new StudentsInSubjectListModel
            {
                Id = entity.Id,
                SubjectId = entity.Subject.Id,
                SubjectName = entity.Subject.Name,
                SubjectAbbreviation = entity.Subject.Abbreviation
            };

    public override StudentsInSubjectDetailModel MapToDetailModel(StudentsInSubjectEntity? entity)
        => entity?.Student is null
            ? StudentsInSubjectDetailModel.Empty
            : new StudentsInSubjectDetailModel
            {
                Id = entity.Id,
                SubjectId = entity.Subject.Id,
                SubjectName = entity.Subject.Name,
                SubjectAbbreviation = entity.Subject.Abbreviation
            };

    public StudentsInSubjectListModel MapToListModel(StudentsInSubjectDetailModel detailModel)
        => new()
        {
            Id = detailModel.Id,
            SubjectId = detailModel.SubjectId,
            SubjectName = detailModel.SubjectName,
            SubjectAbbreviation = detailModel.SubjectAbbreviation
        };

    public void MapToExistingDetailModel(StudentsInSubjectDetailModel existingDetailModel,
        StudentListModel subject)
    {
        existingDetailModel.SubjectId = subject.Id;
        existingDetailModel.SubjectName = subject.Name;
        existingDetailModel.SubjectAbbreviation = subject.Surname;
    }

    public override StudentsInSubjectEntity MapToEntity(StudentsInSubjectDetailModel model)
        => throw new NotImplementedException("This method is unsupported. Use the other overload.");

    public StudentsInSubjectEntity MapToEntity(StudentsInSubjectDetailModel model, Guid studentId)
        => new()
        {
            Id = model.Id,
            SubjectId = model.SubjectId,
            StudentId = studentId,
            Student = null!,
            Subject = null!
        };

    public StudentsInSubjectEntity MapToEntity(StudentsInSubjectListModel model, Guid studentId)
        => new()
        {
            Id = model.Id,
            SubjectId = model.SubjectId,
            StudentId = studentId,
            Student = null!,
            Subject = null!
        };
}