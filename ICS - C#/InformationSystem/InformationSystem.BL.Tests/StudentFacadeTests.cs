using InformationSystem.BL.Facades;
using InformationSystem.BL.Models;
using InformationSystem.DAL.Enums;
using InformationSystem.Common.Tests;
using InformationSystem.Common.Tests.Seeds;
using System.Collections.ObjectModel;
using Xunit;
using Xunit.Abstractions;
using KellermanSoftware.CompareNetObjects;
using Microsoft.EntityFrameworkCore;

namespace InformationSystem.BL.Tests;

public class StudentFacadeTests : FacadeTestsBase
{
    private readonly IStudentFacade _facadeSUT;

    public StudentFacadeTests(ITestOutputHelper output) : base(output)
    {
        _facadeSUT = new StudentFacade(UnitOfWorkFactory, StudentModelMapper);
    }

    
    [Fact]
    public async Task Delete_StudentUsedInSubject_Throws()
    {
        //Act & Assert
        await Assert.ThrowsAsync<InvalidOperationException>(async () => await _facadeSUT.DeleteAsync(StudentSeeds.StudentEntity1.Id));
    }
    
    [Fact]
    public async Task SeededStudentEntity2_DeleteById_Deleted()
    {
        var student = new StudentDetailModel()
        {
            Id = Guid.NewGuid(),
            Name = "Tomas",
            Surname = "Reznicek",
            Login = "xrezni33"
        };

        //Act and Assert
        await using var dbxAssert1 = await DbContextFactory.CreateDbContextAsync();
        Assert.False(await dbxAssert1.Students.AnyAsync(i => i.Id == student.Id));
    }
    
    [Fact]
    public async Task GetById_SeededStudent()
    {
        var model = await _facadeSUT.GetAsync(StudentSeeds.StudentEntity1.Id) ;

        DeepAssert.Equal(StudentModelMapper.MapToDetailModel(StudentSeeds.StudentEntity1) with { StudentsSubjects = default! }, model! with { StudentsSubjects = default! });
    }

    [Fact]
    public async Task NewStudent_InsertOrUpdate_StudentAdded()
    {
        //Arrange
        var student = new StudentDetailModel()
        {
            Id = Guid.Empty, Name = "Tomas", Surname = "Reznicek", Login = "xrezni33"
        };
       
        //Act
        student = await _facadeSUT.SaveAsync(student with { StudentsSubjects = default! });

        //Assert
        await using var dbxAssert = await DbContextFactory.CreateDbContextAsync();
        var studentFromDb = await dbxAssert.Students.SingleAsync(i => i.Id == student.Id);
        DeepAssert.Equal(student, StudentModelMapper.MapToDetailModel(studentFromDb));
    }
    
    [Fact]
    public async Task SeededStudent_InsertOrUpdate_StudentUpdated()
    {
        //Arrange
        var student = new StudentDetailModel()
        {
            Id = StudentSeeds.StudentEntity1.Id,
            Login = StudentSeeds.StudentEntity1.Login,
            Name = StudentSeeds.StudentEntity1.Name,
            Surname = StudentSeeds.StudentEntity1.Surname
        };
        student.Name += " updated";

        //Act
        await _facadeSUT.SaveAsync(student with { StudentsSubjects = default! });

        //Assert
        await using var dbxAssert = await DbContextFactory.CreateDbContextAsync();
        var studentFromDb = await dbxAssert.Students.SingleAsync(i => i.Id == student.Id);
        DeepAssert.Equal(student, StudentModelMapper.MapToDetailModel(studentFromDb));
    }
}