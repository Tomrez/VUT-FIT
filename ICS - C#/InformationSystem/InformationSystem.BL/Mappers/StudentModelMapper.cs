using InformationSystem.BL.Models;
using InformationSystem.DAL.Entities;

namespace InformationSystem.BL.Mappers;

public class StudentModelMapper(StudentsInSubjectModelMapper studentsInSubjectModelMapper, StudentEvaluationModelMapper studentEvaluationModelMapper) :
    ModelBaseMapper<StudentEntity, StudentListModel, StudentDetailModel>
{
    public override StudentListModel MapToListModel(StudentEntity? entity)
        => entity is null
            ? StudentListModel.Empty
            : new StudentListModel
            {
                Id = entity.Id,
                Login = entity.Login,
                Name = entity.Name,
                Surname = entity.Surname,
                PhotoUrl = entity.PhotoUrl,
                StudentsSubjects = studentsInSubjectModelMapper.MapToListModel(entity.Subjects)
                    .ToObservableCollection(),
                StudentEvaluations = studentEvaluationModelMapper.MapToListModel(entity.ActivityEvaluation)
                    .ToObservableCollection()
            };

    public override StudentDetailModel MapToDetailModel(StudentEntity? entity)
        => entity is null
            ? StudentDetailModel.Empty
            : new StudentDetailModel
            {
                Id = entity.Id,
                Login = entity.Login,
                Name = entity.Name,
                Surname = entity.Surname,
                PhotoUrl = entity.PhotoUrl,
                StudentsSubjects = studentsInSubjectModelMapper.MapToListModel(entity.Subjects)
                    .ToObservableCollection(),
                StudentEvaluations = studentEvaluationModelMapper.MapToListModel(entity.ActivityEvaluation)
                    .ToObservableCollection()
            };

    public override StudentEntity MapToEntity(StudentDetailModel model)
        => new()
        {
            Id = model.Id,
            Name = model.Name,
            Surname = model.Surname,
            Login = model.Login,
            PhotoUrl = model.PhotoUrl
        };
}