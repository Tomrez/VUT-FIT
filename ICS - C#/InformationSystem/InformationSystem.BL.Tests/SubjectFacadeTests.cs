using InformationSystem.BL.Facades;
using InformationSystem.BL.Models;
using InformationSystem.DAL.Enums;
using InformationSystem.Common.Tests;
using InformationSystem.Common.Tests.Seeds;
using System.Collections.ObjectModel;
using Xunit;
using Xunit.Abstractions;
using KellermanSoftware.CompareNetObjects;
using InformationSystem.DAL.Entities;

namespace InformationSystem.BL.Tests;

public class SubjectFacadeTests : FacadeTestsBase
{
    private readonly ISubjectFacade _facadeSUT;

    public SubjectFacadeTests(ITestOutputHelper output) : base(output)
    {
        _facadeSUT = new SubjectFacade(UnitOfWorkFactory, SubjectModelMapper);
    }

    [Fact]
    public async Task Create_WithWithoutStudent_EqualsCreated()
    {
        //Arrange
        var model = new SubjectDetailModel()
        {
            Id = Guid.NewGuid(),
            Name = "Test1",
            Abbreviation = "TST"
        };

        //Act
        var returnedModel = await _facadeSUT.SaveAsync(model);

        //Assert
        FixIds(model, returnedModel);
        DeepAssert.Equal(model, returnedModel);
    }

    [Fact]
    public async Task GetById_FromSeeded_EqualsSeeded()
    {
        //Arrange
        var detailModel = SubjectModelMapper.MapToDetailModel(SubjectSeeds.SubjectEntity1);

        //Act
        var returnedModel = await _facadeSUT.GetAsync(detailModel.Id);

        //Assert
        DeepAssert.Equal(detailModel with { Activities = default! }, returnedModel! with { Activities = default! });
    }

    [Fact]
    public async Task GetAll_FromSeeded_ContainsSeeded()
    {
        //Arrange
        var listModel = SubjectModelMapper.MapToListModel(SubjectSeeds.SubjectEntity1);

        //Act
        var returnedModel = await _facadeSUT.GetAsync();

        //Assert
        Assert.Contains(listModel, returnedModel);
    }

    [Fact]
    public async Task Update_Name_FromSeeded_Updated()
    {
        //Arrange
        var detailModel = SubjectModelMapper.MapToDetailModel(SubjectSeeds.SubjectEntity1);
        detailModel.Name = "New subject name";

        //Act
        await _facadeSUT.SaveAsync(detailModel with { Activities = default! });

        //Assert
        var returnedModel = await _facadeSUT.GetAsync(detailModel.Id);
        DeepAssert.Equal(detailModel, returnedModel, nameof(detailModel.Activities));
    }

    [Fact]
    public async Task Update_FromSeeded_DoesNotThrow()
    {
        //Arrange
        var model = SubjectModelMapper.MapToDetailModel(SubjectSeeds.SubjectEntity1);
        model.Name = "New subject name";

        //Act and Assert
        await _facadeSUT.SaveAsync(model with { Activities = default! });
    }

    [Fact]
    public async Task Update_RemoveStudents_FromSeeded_NotUpdated()
    {
        //Arrange
        var detailModel = SubjectModelMapper.MapToDetailModel(SubjectSeeds.SubjectEntity1);

        //Act
        await _facadeSUT.SaveAsync(detailModel with { Activities = default! });

        //Assert
        var returnedModel = await _facadeSUT.GetAsync(detailModel.Id);
        DeepAssert.Equal(detailModel with { Activities = default! }, returnedModel! with { Activities = default! });
    }

    [Fact]
    public async Task Update_DeleteOneOfStudents_FromSeeded_Updated()
    {
        //Arrange
        var detailModel = SubjectModelMapper.MapToDetailModel(SubjectSeeds.SubjectEntity1);
        detailModel.Name = "xd";

        //Act
        await _facadeSUT.SaveAsync(detailModel);

        //Assert
        var returnedModel = await _facadeSUT.GetAsync(detailModel.Id);
        DeepAssert.Equal(SubjectModelMapper.MapToDetailModel(SubjectSeeds.SubjectEntity1) with { Activities = default!, Name = "xd" }, returnedModel! with { Activities = default! });
    }

    private static void FixIds(SubjectDetailModel expectedModel, SubjectDetailModel returnedModel)
    {
        returnedModel.Id = expectedModel.Id;

        foreach (var activitiesModel in returnedModel.Activities)
        {
            var activityDetailModel = expectedModel.Activities.FirstOrDefault(i =>
                i.ActivityStart == activitiesModel.ActivityStart
                && i.ActivityEnd == activitiesModel.ActivityEnd
                && i.ActivityType == activitiesModel.ActivityType);

            if (activityDetailModel != null)
            {
                activitiesModel.Id = activityDetailModel.Id;
            }
        }
    }
}
